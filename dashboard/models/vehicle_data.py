import threading

class VehicleDataStore:
    """Thread-safe storage for vehicle data"""
    _instance = None
    _lock = threading.Lock()
    
    def __new__(cls):
        # Singleton pattern
        if cls._instance is None:
            with cls._lock:
                if cls._instance is None:
                    cls._instance = super(VehicleDataStore, cls).__new__(cls)
                    cls._instance._data = {}
        return cls._instance
    
    def update(self, key, value):
        """Update data for a key"""
        with self._lock:
            self._data[key] = value
    
    def get(self, key):
        """Get data for a key"""
        with self._lock:
            return self._data.get(key)
    
    def get_all(self):
        """Get all data"""
        with self._lock:
            # Return a copy to avoid modification issues
            return self._data.copy()