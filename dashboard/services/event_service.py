import threading
import queue
import logging
import json
import time
from vehicle.client import VehicleClient
from models.vehicle_data import VehicleDataStore

logger = logging.getLogger(__name__)


class EventService:
    _instance = None

    def __init__(self, gateway_url):
        if EventService._instance is not None:
            raise Exception("EventService is a singleton. Use get_instance() instead.")
        self.client = VehicleClient(gateway_url)
        self.data_store = VehicleDataStore()
        self.event_queue = queue.Queue()
        self.active_threads = {}

    @classmethod
    def get_instance(cls, gateway_url=None):
        if cls._instance is None and gateway_url is not None:
            cls._instance = cls(gateway_url)
        return cls._instance

    
    def start_fuel_level_stream(self, interval=2, max_updates=10):
        """Start streaming fuel level updates"""
        # Check if already streaming
        if "fuel_level_stream" in self.active_threads and self.active_threads["fuel_level_stream"].is_alive():
            return {"status": "warning", "message": "Fuel level stream already active"}
        logger.info(f"New thread")
        # Create and start a new thread for streaming
        thread = threading.Thread(
            target=self._fuel_level_update_thread,
            args=(interval, max_updates),
            daemon=True
        )
        thread.start()
        
        # Store thread reference
        self.active_threads["fuel_level_stream"] = thread
        logger.info(f"Streaming started")
        return {"status": "success", "message": "Streaming started"}
    
    def _fuel_level_update_thread(self, interval, max_updates):
        """Thread function to get fuel level updates"""
        try:
            for update in self.client.stream_fuel_level(interval, max_updates):
                # Store the latest value
                self.data_store.update("fuel_level", {
                    "value": f"{update['data']['level_percent']}%",
                    "timestamp": time.time(),
                    "raw_data": update
                })
                
                logger.info(f"New update: {update}")
                # Add to event queue for any listening clients
                self.event_queue.put({
                    "event": "fuel_level_update",
                    "data": update
                })
                                
        except Exception as e:
            logger.error(f"Stream error: {e}")
            self.event_queue.put({
                "event": "error",
                "data": {"message": str(e)}
            })
    
    def get_events(self):
        """Generator for event stream"""
        while True:
            try:
                # Get message from queue with timeout
                message = self.event_queue.get(timeout=1)
                yield f"data: {json.dumps(message)}\n\n"
            except queue.Empty:
                # Send keepalive comment
                yield ": keepalive\n\n"