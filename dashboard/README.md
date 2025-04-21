# Vehicle Dashboard

A Flask-based web dashboard for monitoring and controlling vehicle data through a V2C (Vehicle-to-Cloud) gateway.

## Overview

This dashboard provides a web interface to:
- Monitor real-time vehicle data (fuel level, headlight state)
- Control vehicle features (headlights)
- Stream vehicle data updates
- View historical vehicle data

## Architecture

The application is built using:
- Flask 3.1.0 as the web framework
- Server-Sent Events (SSE) for real-time updates
- RESTful API endpoints for data access and control
- Modular service layer for business logic

### Key Components

- **API Routes** (`/api/*`):
  - `/request_data` - Request specific vehicle data
  - `/vehicle_data` - Get all stored vehicle data
  - `/stream/fuel_level` - Start streaming fuel level data
  - `/events` - SSE endpoint for real-time updates
  - `/headlights` - Get and set headlight state

- **Services**:
  - `VehicleDataService` - Handles vehicle data operations
  - `EventService` - Manages real-time event streaming

- **Frontend**:
  - Single-page dashboard interface
  - Real-time updates via SSE
  - Interactive controls for vehicle features

## Configuration

The application can be configured through environment variables:

- `DEBUG` - Enable/disable debug mode (default: True)
- `PORT` - Server port (default: 5000)
- `VEHICLE_GATEWAY_URL` - URL of the V2C gateway (default: http://localhost:8080)

## Installation

1. Create and activate a virtual environment:
```bash
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
```

2. Install dependencies:
```bash
pip install -r requirements.txt
```

3. Run the application:
```bash
python app.py
```

The dashboard will be available at `http://localhost:5000` (or the configured port).

## API Documentation

### Request Vehicle Data
```http
POST /api/request_data
Content-Type: application/json

{
    "key": "fuel_level"
}
```

### Get Vehicle Data
```http
GET /api/vehicle_data
```

### Stream Fuel Level
```http
GET /api/stream/fuel_level?interval=2&max_updates=10
```

### Headlight Control
```http
GET /api/headlights
PUT /api/headlights
Content-Type: application/json

{
    "turn_on": true
}
```

## Development Status

The dashboard currently supports:
- Real-time fuel level monitoring
- Headlight state control
- Basic vehicle data storage and retrieval
- Server-Sent Events for live updates

Future improvements could include:
- Support for additional vehicle data points
- Enhanced error handling and logging
- User authentication and authorization
- Historical data visualization
- More comprehensive vehicle controls

## Dependencies

- Flask 3.1.0
- requests 2.32.3
- sseclient-py 1.8.0
- Other dependencies listed in requirements.txt 