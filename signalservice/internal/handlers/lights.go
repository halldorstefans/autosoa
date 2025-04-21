package handlers

import (
	"context"
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	"github.com/halldorstefans/signalservice/internal/signal"
)

type LightHandler struct {
	client    signal.Service
	vehicleID string
}

func NewLightHandler(client signal.Service, vehicleID string) *LightHandler {
	return &LightHandler{client: client, vehicleID: vehicleID}
}

func (h *LightHandler) GetHeadlights(w http.ResponseWriter, r *http.Request) {
	ctx, cancel := context.WithTimeout(r.Context(), 2*time.Second)
	defer cancel()

	state, err := h.client.GetHeadlightState(ctx)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	response := map[string]interface{}{
		"status":     "success",
		"vehicle_id": h.vehicleID,
		"data_key":   "headlights",
		"data":       state,
		"timestamp":  time.Now().Unix(),
		"request_id": r.Header.Get("X-Request-ID"),
	}
	json.NewEncoder(w).Encode(response)
}

func (h *LightHandler) SetHeadlights(w http.ResponseWriter, r *http.Request) {
	ctx, cancel := context.WithTimeout(r.Context(), 2*time.Second)
	defer cancel()

	var req struct {
		TurnOn bool `json:"turn_on"`
	}
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		http.Error(w, "Invalid request body", http.StatusBadRequest)
		return
	}

	success, err := h.client.SetHeadlight(ctx, req.TurnOn)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	response := map[string]interface{}{
		"status":     success,
		"vehicle_id": h.vehicleID,
		"command":    fmt.Sprintf("turn_headlights_%s", map[bool]string{true: "on", false: "off"}[req.TurnOn]),
		"timestamp":  time.Now().Unix(),
		"request_id": r.Header.Get("X-Request-ID"),
	}
	json.NewEncoder(w).Encode(response)
}
