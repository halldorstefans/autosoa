class VehicleError(Exception):
    """Base exception for vehicle client errors"""
    pass

class VehicleConnectionError(VehicleError):
    """Exception raised for connection-related errors"""
    pass

class VehicleRequestError(VehicleError):
    """Exception raised for request/response errors"""
    pass