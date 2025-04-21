import logging
from flask import Blueprint, request, jsonify, current_app
from services.data_service import VehicleDataService
from services.event_service import EventService

logger = logging.getLogger(__name__)

api_bp = Blueprint('api', __name__)


@api_bp.before_app_request
def initialize_services():
    gateway_url = current_app.config['VEHICLE_GATEWAY_URL']
    VehicleDataService.get_instance(gateway_url)
    EventService.get_instance(gateway_url)


@api_bp.route('/request_data', methods=['POST'])
def request_data():
    """Request data from vehicle via V2C gateway"""
    data_key = request.json.get('key')
    
    if not data_key:
        return jsonify({"error": "No data key provided"}), 400
    
    # Currently only supporting fuel_level
    if data_key != "fuel_level":
        return jsonify({"error": f"Unsupported data key: {data_key}"}), 400
    
    try:
        result = VehicleDataService.get_instance().request_fuel_level()
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@api_bp.route('/vehicle_data', methods=['GET'])
def get_vehicle_data():
    """Get all stored vehicle data"""
    return jsonify(VehicleDataService.get_instance().get_all_data())

@api_bp.route('/stream/fuel_level')
def stream_fuel_level():
    """Start streaming fuel level data from the vehicle"""
    # Get parameters
    interval = int(request.args.get('interval', 2))
    max_updates = int(request.args.get('max_updates', 10))
    
    try:
        result = EventService.get_instance().start_fuel_level_stream(interval, max_updates)
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@api_bp.route('/events')
def event_stream():
    """SSE endpoint for real-time updates"""
    logger.info("Client connected to /events endpoint")
    response = current_app.response_class(
        EventService.get_instance().get_events(),
        mimetype='text/event-stream'
    )
    response.headers['Cache-Control'] = 'no-cache'
    response.headers['Connection'] = 'keep-alive'
    return response

@api_bp.route('/headlights', methods=['GET'])
def get_headlight_state():
    try:
        result = VehicleDataService.get_instance().get_headlight_state()
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@api_bp.route('/headlights', methods=['PUT'])
def set_headlight_state():
    try:
        value = request.json.get('turn_on')
        
        if not isinstance(value, bool):
            return jsonify({"error": "Expected a boolean for 'turn_on'"}), 400
            
        result = VehicleDataService.get_instance().set_headlight_state(value)
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500