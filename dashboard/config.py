import os

class Config:
    DEBUG = os.environ.get('DEBUG', True)
    PORT = int(os.environ.get('PORT', 5000))
    VEHICLE_GATEWAY_URL = os.environ.get('VEHICLE_GATEWAY_URL', 'http://localhost:8080')