import time
import logging
from vehicle.client import VehicleClient
from models.vehicle_data import VehicleDataStore

logger = logging.getLogger(__name__)


class VehicleDataService:
    _instance = None

    def __init__(self, gateway_url):
        if VehicleDataService._instance is not None:
            raise Exception("VehicleDataService is a singleton. Use get_instance() instead.")
        self.client = VehicleClient(gateway_url)
        self.data_store = VehicleDataStore()

    @classmethod
    def get_instance(cls, gateway_url=None):
        if cls._instance is None and gateway_url is not None:
            cls._instance = cls(gateway_url)
        return cls._instance
    
    def get_all_data(self):
        """Return all stored vehicle data"""
        return self.data_store.get_all()
    
    def request_fuel_level(self):
        """Request fuel level data from vehicle"""
        try:
            response = self.client.get_fuel_level()
            
            # Store the received data
            self.data_store.update("fuel_level", {
                "value": f"{response['data']['level_percent']}%",
                "timestamp": time.time(),
                "raw_data": response
            })
            
            return {
                "status": "success", 
                "data": {
                    "key": "fuel_level",
                    "value": f"{response['data']['level_percent']}%"
                }
            }
        except Exception as e:
            logger.error(f"Error requesting fuel level: {str(e)}")
            raise
    
    def get_headlight_state(self):
        """Get current headlight state from vehicle"""
        try:
            response = self.client.get_headlight_state()
            return {
                "status": "success",
                "data": {
                    "key": "headlights",
                    "value": response['data']['headlight_state']
                }
            }
        except Exception as e:
            logger.error(f"Error getting headlight state: {str(e)}")
            raise
    
    def set_headlight_state(self, state):
        """Set headlight state on vehicle"""
        try:
            if not isinstance(state, bool):
                raise ValueError("Expected a boolean for headlight state")
                
            response = self.client.set_headlight_state(state)
            
            if response:
                return {
                    "status": "success",
                    "data": {
                        "key": "headlights",
                        "value": state
                    }
                }
            else:
                raise Exception("Failed to set headlight state")
                
        except Exception as e:
            logger.error(f"Error setting headlight state: {str(e)}")
            raise