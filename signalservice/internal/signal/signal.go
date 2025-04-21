package signal

import (
	"context"
	"fmt"
	"io"
	"log"

	pb "github.com/halldorstefans/signalservice/proto"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

type FuelLevel struct {
	LevelPercent float32 `json:"level_percent"`
	TimestampMs  uint64  `json:"timestamp_ms"`
	Status       int32   `json:"status"`
}

type HeadlightState struct {
	HeadlightOn bool `json:"headlight_state"`
}

type HeadlightResponse struct {
	Status bool `json:"status"`
}

type Service interface {
	GetFuelLevel(ctx context.Context) (*FuelLevel, error)
	StreamFuelLevel(ctx context.Context, intervalSeconds, maxUpdates int) (<-chan *FuelLevel, error)
	GetHeadlightState(ctx context.Context) (*HeadlightState, error)
	SetHeadlight(ctx context.Context, turnOn bool) (*HeadlightResponse, error)
	Close() error
}

type service struct {
	conn        *grpc.ClientConn
	fuelClient  pb.OBDServiceClient
	lightClient pb.LightingServiceClient
	vehicleID   string
}

func NewService(serverAddr, vehicleID string) (Service, error) {
	conn, err := grpc.NewClient(serverAddr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		return nil, fmt.Errorf("failed to connect: %w", err)
	}
	return &service{
		conn:        conn,
		fuelClient:  pb.NewOBDServiceClient(conn),
		lightClient: pb.NewLightingServiceClient(conn),
		vehicleID:   vehicleID,
	}, nil
}

func (c *service) Close() error {
	return c.conn.Close()
}

func (c *service) GetFuelLevel(ctx context.Context) (*FuelLevel, error) {
	response, err := c.fuelClient.GetFuelLevel(ctx, &pb.FuelLevelRequest{VehicleId: c.vehicleID})
	if err != nil {
		return nil, fmt.Errorf("error getting fuel level: %w", err)
	}
	return &FuelLevel{
		LevelPercent: response.LevelPercent,
		TimestampMs:  response.TimestampMs,
		Status:       response.Status,
	}, nil
}

func (c *service) StreamFuelLevel(ctx context.Context, intervalSeconds, maxUpdates int) (<-chan *FuelLevel, error) {
	req := &pb.FuelLevelStreamRequest{
		VehicleId:       c.vehicleID,
		IntervalSeconds: uint32(intervalSeconds),
	}

	stream, err := c.fuelClient.StreamFuelLevel(ctx, req)
	if err != nil {
		return nil, fmt.Errorf("error setting up stream: %w", err)
	}

	updates := make(chan *FuelLevel)
	go func() {
		defer close(updates)
		count := 0
		for maxUpdates <= 0 || count < maxUpdates {
			res, err := stream.Recv()
			log.Printf("Thread stream update")
			if err == io.EOF {
				break
			}
			if err != nil {
				// Stream ends on error; client will handle it.
				return
			}
			updates <- &FuelLevel{
				LevelPercent: res.LevelPercent,
				TimestampMs:  res.TimestampMs,
				Status:       res.Status,
			}
			count++
		}
	}()

	return updates, nil
}

func (c *service) GetHeadlightState(ctx context.Context) (*HeadlightState, error) {
	res, err := c.lightClient.GetHeadlightState(ctx, &pb.GetHeadlightStateRequest{})
	if err != nil {
		return nil, fmt.Errorf("error getting headlight state: %w", err)
	}
	return &HeadlightState{
		HeadlightOn: res.IsOn,
	}, nil
}

func (c *service) SetHeadlight(ctx context.Context, turnOn bool) (*HeadlightResponse, error) {
	res, err := c.lightClient.SetHeadlight(ctx, &pb.SetHeadlightRequest{TurnOn: turnOn})
	if err != nil {
		return nil, fmt.Errorf("error setting headlight state: %w", err)
	}
	return &HeadlightResponse{
		Status: res.Success,
	}, nil
}
