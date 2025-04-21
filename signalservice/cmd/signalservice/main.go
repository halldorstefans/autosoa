package main

import (
	"log"

	"github.com/halldorstefans/signalservice/internal/config"
	"github.com/halldorstefans/signalservice/internal/gateway"
	"github.com/halldorstefans/signalservice/internal/signal"
)

func main() {
	cfg, err := config.Load()
	if err != nil {
		log.Fatalf("Config error: %v", err)
	}

	obClient, err := signal.NewService(cfg.OBServerAddr, cfg.DefaultVehicle)
	if err != nil {
		log.Fatalf("Failed to create OB client: %v", err)
	}

	srv := gateway.New(cfg.GatewayPort, cfg.DefaultVehicle, obClient)
	srv.Run()
}
