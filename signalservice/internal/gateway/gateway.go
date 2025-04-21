package gateway

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/go-chi/chi/v5"
	"github.com/go-chi/chi/v5/middleware"
	"github.com/halldorstefans/signalservice/internal/handlers"
	signalsvc "github.com/halldorstefans/signalservice/internal/signal"
	mw "github.com/halldorstefans/signalservice/pkg/middleware"
)

type Server struct {
	port           int
	defaultVehicle string
	signalsClient  signalsvc.Service
}

func New(port int, vehicleID string, client signalsvc.Service) *Server {
	return &Server{
		port:           port,
		defaultVehicle: vehicleID,
		signalsClient:  client,
	}
}

func (s *Server) Run() {
	r := chi.NewRouter()
	r.Use(middleware.RequestID)
	r.Use(middleware.Logger)
	r.Use(middleware.Recoverer)
	r.Use(mw.JSONContentType)

	fuelHandler := handlers.NewFuelHandler(s.signalsClient, s.defaultVehicle)
	lightHandler := handlers.NewLightHandler(s.signalsClient, s.defaultVehicle)

	r.Route("/api/v1/vehicle", func(r chi.Router) {
		r.Get("/data/fuel_level", fuelHandler.GetFuelLevel)
		r.Get("/stream/fuel_level", fuelHandler.StreamFuelLevel)
		r.Route("/lighting/headlights", func(r chi.Router) {
			r.Get("/", lightHandler.GetHeadlights)
			r.Put("/", lightHandler.SetHeadlights)
		})
	})

	srv := &http.Server{
		Addr:    fmt.Sprintf(":%d", s.port),
		Handler: r,
	}

	go func() {
		log.Printf("V2C Gateway starting on :%d...", s.port)
		if err := srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			log.Fatalf("ListenAndServe(): %v", err)
		}
	}()

	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)
	<-quit
	log.Println("Shutting down server...")
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()
	if err := srv.Shutdown(ctx); err != nil {
		log.Fatalf("Server Shutdown Failed:%+v", err)
	}
	log.Println("Server exited")
}
