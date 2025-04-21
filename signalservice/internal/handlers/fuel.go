package handlers

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"strconv"
	"time"

	"github.com/halldorstefans/signalservice/internal/signal"
)

type FuelHandler struct {
	client    signal.Service
	vehicleID string
}

func NewFuelHandler(client signal.Service, vehicleID string) *FuelHandler {
	return &FuelHandler{client: client, vehicleID: vehicleID}
}

func (h *FuelHandler) GetFuelLevel(w http.ResponseWriter, r *http.Request) {
	ctx, cancel := context.WithTimeout(r.Context(), 2*time.Second)
	defer cancel()

	data, err := h.client.GetFuelLevel(ctx)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	response := map[string]interface{}{
		"status":     "success",
		"vehicle_id": h.vehicleID,
		"data_key":   "fuel_level",
		"data":       data,
		"timestamp":  time.Now().Unix(),
		"request_id": r.Header.Get("X-Request-ID"),
	}
	json.NewEncoder(w).Encode(response)
}

func (h *FuelHandler) StreamFuelLevel(w http.ResponseWriter, r *http.Request) {
	log.Printf("Received stream fuel level request")
	interval := 2
	if v := r.URL.Query().Get("interval"); v != "" {
		if i, err := strconv.Atoi(v); err == nil && i > 0 {
			interval = i
		}
	}

	maxUpdates := 10
	if v := r.URL.Query().Get("max_updates"); v != "" {
		if m, err := strconv.Atoi(v); err == nil && m > 0 {
			maxUpdates = m
		}
	}

	ctx, cancel := context.WithCancel(r.Context())
	defer cancel()

	updates, err := h.client.StreamFuelLevel(ctx, interval, maxUpdates)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "text/event-stream")
	w.Header().Set("Cache-Control", "no-cache")
	w.Header().Set("Connection", "keep-alive")
	w.Header().Set("Access-Control-Allow-Origin", "*")

	flusher, ok := w.(http.Flusher)
	if !ok {
		http.Error(w, "Streaming not supported", http.StatusInternalServerError)
		return
	}

	for update := range updates {
		log.Printf("Received stream fuel level update")
		response := map[string]interface{}{
			"event":      "fuel_level_update",
			"vehicle_id": h.vehicleID,
			"data":       update,
			"timestamp":  time.Now().Unix(),
		}
		data, err := json.Marshal(response)
		if err != nil {
			log.Printf("Error marshaling JSON: %v", err)
			continue
		}
		fmt.Fprintf(w, "data: %s\n\n", data)
		flusher.Flush()
	}
}
