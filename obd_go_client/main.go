package main

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"time"

	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
	pb "github.com/halldorstefans/obdclient/proto"
)

func main() {
	// Set up a connection to the server
	conn, err := grpc.Dial("localhost:50051", grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf("Failed to connect: %v", err)
	}
	defer conn.Close()

	// Create a client
	client := pb.NewOBDServiceClient(conn)

	// Vehicle ID to use in our requests
	vehicleID := "VIN123456789"

	// Get a single fuel level reading
	fmt.Println("Getting fuel level...")
	getSingleReading(client, vehicleID)

	// Stream fuel level updates
	fmt.Println("\nStreaming fuel level updates (every 2 seconds)...")
	streamReadings(client, vehicleID, 2)
}

// Get a single fuel level reading
func getSingleReading(client pb.OBDServiceClient, vehicleID string) {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	// Make the request
	response, err := client.GetFuelLevel(ctx, &pb.FuelLevelRequest{VehicleId: vehicleID})
	if err != nil {
		log.Fatalf("Error getting fuel level: %v", err)
	}

	// Print regular output
	fmt.Printf("Fuel level: %.2f%%\n", response.LevelPercent)
	fmt.Printf("Timestamp: %d\n", response.TimestampMs)

	// Create and print JSON format
	jsonData := map[string]interface{}{
		"level_percent": response.LevelPercent,
		"timestamp_ms":  response.TimestampMs,
		"status":        response.Status,
	}
	jsonBytes, _ := json.Marshal(jsonData)
	fmt.Printf("JSON: %s\n", string(jsonBytes))
}

// Stream fuel level updates
func streamReadings(client pb.OBDServiceClient, vehicleID string, intervalSeconds int) {
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	// Set up streaming request
	request := &pb.FuelLevelStreamRequest{
		VehicleId:       vehicleID,
		IntervalSeconds: uint32(intervalSeconds),
	}

	// Make the streaming call
	stream, err := client.StreamFuelLevel(ctx, request)
	if err != nil {
		log.Fatalf("Error setting up stream: %v", err)
	}

	// Read the stream (limit to 10 readings for demo)
	count := 0
	for count < 10 {
		response, err := stream.Recv()
		if err == io.EOF {
			break // End of stream
		}
		if err != nil {
			log.Fatalf("Error reading stream: %v", err)
		}

		// Print regular output
		fmt.Printf("Fuel level update: %.2f%%\n", response.LevelPercent)

		// Create and print JSON format
		jsonData := map[string]interface{}{
			"level_percent": response.LevelPercent,
			"timestamp_ms":  response.TimestampMs,
			"status":        response.Status,
		}
		jsonBytes, _ := json.Marshal(jsonData)
		fmt.Printf("JSON: %s\n", string(jsonBytes))

		count++
	}
}

