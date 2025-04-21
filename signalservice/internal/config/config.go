package config

import (
	"fmt"
	"log"
	"os"
	"strconv"
)

type Config struct {
	OBServerAddr   string
	DefaultVehicle string
	GatewayPort    int
}

func Load() (*Config, error) {
	addr := getEnv("OB_SERVER_ADDR", "localhost:50051")
	if addr == "" {
		return nil, fmt.Errorf("OB_SERVER_ADDR is required but not set")
	}

	vehicle := getEnv("DEFAULT_VEHICLE", "VIN123456789")
	if vehicle == "" {
		return nil, fmt.Errorf("DEFAULT_VEHICLE is required but not set")
	}

	port := getEnvAsInt("GATEWAY_PORT", 8080)
	if port == 0 {
		return nil, fmt.Errorf("GATEWAY_PORT cannot be zero")
	}

	cfg := &Config{
		OBServerAddr:   addr,
		DefaultVehicle: vehicle,
		GatewayPort:    port,
	}
	return cfg, nil
}

// Helpers
func getEnv(key string, defaultVal string) string {
	if val := os.Getenv(key); val != "" {
		return val
	}
	return defaultVal
}

func getEnvAsInt(key string, defaultVal int) int {
	valStr := getEnv(key, "")
	if valStr == "" {
		return defaultVal
	}
	val, err := strconv.Atoi(valStr)
	if err != nil {
		log.Printf("Warning: invalid integer for %s: %v. Using default %d", key, err, defaultVal)
		return defaultVal
	}
	return val
}
