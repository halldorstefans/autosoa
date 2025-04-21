# cloud_simulation/vehicle_client.py
import requests
import json
import logging
import time
import sseclient # You'll need to install this: pip install sseclient-py

from .exceptions import VehicleConnectionError, VehicleRequestError

logger = logging.getLogger(__name__)

class VehicleClient:
    """Client for communicating with the V2C Gateway"""
    
    def __init__(self, gateway_url="http://localhost:8080", timeout=10, max_retries=3, retry_delay=1):
        """Initialize the vehicle client
        
        Args:
            gateway_url: Base URL for the vehicle gateway
            timeout: Request timeout in seconds
            max_retries: Maximum number of retry attempts for failed requests
            retry_delay: Delay between retries in seconds
        """
        self.gateway_url = gateway_url
        self.timeout = timeout
        self.max_retries = max_retries
        self.retry_delay = retry_delay
        logger.info(f"VehicleClient initialized with gateway: {gateway_url}")

    def _make_request(self, method, endpoint, **kwargs):
        """Make an HTTP request with retries and error handling
        
        Args:
            method: HTTP method (get, post, put, etc.)
            endpoint: API endpoint path
            **kwargs: Additional arguments to pass to requests
            
        Returns:
            JSON response data
            
        Raises:
            VehicleConnectionError: If connection to the gateway fails
            VehicleRequestError: If request fails or returns an error
        """
        url = f"{self.gateway_url}{endpoint}"
        request_kwargs = {
            'timeout': self.timeout,
            **kwargs
        }
        
        # Add default headers if not provided
        if 'headers' not in request_kwargs:
            request_kwargs['headers'] = {'Accept': 'application/json'}
            
        # Initialize variables for retry logic
        attempts = 0
        last_exception = None
        
        while attempts < self.max_retries:
            try:
                logger.debug(f"Making {method.upper()} request to {url}, attempt {attempts+1}/{self.max_retries}")
                response = requests.request(method, url, **request_kwargs)
                
                # Check for HTTP errors
                response.raise_for_status()
                
                # Parse JSON response
                try:
                    data = response.json()
                except json.JSONDecodeError as e:
                    raise VehicleRequestError(f"Invalid JSON response: {e}")
                
                # Check for API-level errors
                if data.get('error'):
                    raise VehicleRequestError(f"API error: {data['error']}")
                    
                return data
                
            except requests.exceptions.VehicleConnectionError as e:
                last_exception = VehicleConnectionError(f"Failed to connect to vehicle gateway: {e}")
                logger.warning(f"Connection error (attempt {attempts+1}/{self.max_retries}): {e}")
            except requests.exceptions.Timeout as e:
                last_exception = VehicleConnectionError(f"Request timed out: {e}")
                logger.warning(f"Timeout error (attempt {attempts+1}/{self.max_retries}): {e}")
            except requests.exceptions.HTTPError as e:
                # Don't retry on client errors (4xx)
                if response.status_code >= 400 and response.status_code < 500:
                    raise VehicleRequestError(f"HTTP error {response.status_code}: {e}")
                last_exception = VehicleRequestError(f"HTTP error {response.status_code}: {e}")
                logger.warning(f"HTTP error (attempt {attempts+1}/{self.max_retries}): {e}")
            except Exception as e:
                last_exception = VehicleRequestError(f"Unexpected error: {e}")
                logger.warning(f"Unexpected error (attempt {attempts+1}/{self.max_retries}): {e}")
            
            # Increment attempt counter and delay before retry
            attempts += 1
            if attempts < self.max_retries:
                time.sleep(self.retry_delay)
        
        # If we get here, all retries failed
        logger.error(f"All {self.max_retries} attempts failed")
        raise last_exception or VehicleConnectionError("Failed to communicate with vehicle gateway")

    def get_fuel_level(self):
        """Get the current fuel level
        
        Returns:
            Dictionary containing fuel level data
            
        Raises:
            VehicleConnectionError: If connection to the gateway fails
            VehicleRequestError: If request fails or returns an error
        """
        try:
            endpoint = "/api/v1/vehicle/data/fuel_level"
            return self._make_request('get', endpoint)
        except Exception as e:
            logger.error(f"Error getting fuel level: {e}")
            raise
    
    def stream_fuel_level(self, interval=2, max_updates=10, callback=None):
        """Stream fuel level updates
        
        Args:
            interval: Update interval in seconds
            max_updates: Maximum number of updates to receive
            callback: Function to call with each update
            
        Returns:
            Generator yielding updates if no callback is provided
            
        Raises:
            VehicleConnectionError: If connection to the gateway fails
            VehicleRequestError: If request fails or returns an error
        """
        endpoint = f"/api/v1/vehicle/stream/fuel_level?interval={interval}&max_updates={max_updates}"
        headers = {'Accept': 'text/event-stream'}
        
        try:
            logger.debug(f"Starting fuel level stream (interval={interval}s, max_updates={max_updates})")
            
            response = self._make_stream_request('get', endpoint, headers=headers)

            client = sseclient.SSEClient(response)
                                        
            for event in client.events():
                try:
                    data = json.loads(event.data)
                    if callback is None:
                        yield data
                    else:
                        callback(data)
                except json.JSONDecodeError as e:
                    logger.warning(f"Invalid JSON in SSE event: {e}")
                except Exception as e:
                    logger.error(f"Error processing SSE event: {e}")
                    raise VehicleRequestError(f"Stream processing error: {e}")
        except Exception as e:
            logger.error(f"Stream error: {e}")
            raise

    def _make_stream_request(self, method, endpoint, **kwargs):
        """Make a streaming request with error handling
        
        Args:
            method: HTTP method (get, post, put, etc.)
            endpoint: API endpoint path
            **kwargs: Additional arguments to pass to requests
            
        Returns:
            Response object for streaming
            
        Raises:
            VehicleConnectionError: If connection to the gateway fails
            VehicleRequestError: If request fails or returns an error
        """
        url = f"{self.gateway_url}{endpoint}"
        request_kwargs = {
            'stream': True,  # Ensure streaming is enabled
            'timeout': self.timeout,
            **kwargs
        }
        
        try:
            logger.debug(f"Making streaming {method.upper()} request to {url}")
            
            response = requests.request(method, url, **request_kwargs)
            
            # Check for HTTP errors
            response.raise_for_status()
            return response
            
        except requests.exceptions.VehicleConnectionError as e:
            logger.error(f"Connection error in stream request: {e}")
            raise VehicleConnectionError(f"Failed to connect to vehicle gateway: {e}")
        except requests.exceptions.Timeout as e:
            logger.error(f"Timeout error in stream request: {e}")
            raise VehicleConnectionError(f"Stream request timed out: {e}")
        except requests.exceptions.HTTPError as e:
            logger.error(f"HTTP error in stream request: {response.status_code} {e}")
            raise VehicleRequestError(f"HTTP error {response.status_code}: {e}")
        except Exception as e:
            logger.error(f"Unexpected error in stream request: {e}")
            raise VehicleRequestError(f"Unexpected error in stream request: {e}")


    def get_headlight_state(self):
        """Get the current headlight state
        
        Returns:
            Dictionary containing headlight state data
            
        Raises:
            VehicleConnectionError: If connection to the gateway fails
            VehicleRequestError: If request fails or returns an error
        """
        try:
            endpoint = "/api/v1/vehicle/lighting/headlights"
            logger.debug("Getting headlight state")
            return self._make_request('get', endpoint)
        except Exception as e:
            logger.error(f"Error getting headlight state: {e}")
            raise

    
    def set_headlight_state(self, state=False):
        """Set the headlight state
        
        Args:
            state: Boolean indicating desired headlight state (True=on, False=off)
            
        Returns:
            Dictionary containing operation result
            
        Raises:
            VehicleConnectionError: If connection to the gateway fails
            VehicleRequestError: If request fails or returns an error
            ValueError: If state is not a boolean
        """
        try:
            # Validate input
            if not isinstance(state, bool):
                raise ValueError("Headlight state must be a boolean")
                
            endpoint = "/api/v1/vehicle/lighting/headlights"
            logger.debug(f"Setting headlight state to: {state}")
            
            return self._make_request('put', endpoint, json={'turn_on': state})
        except ValueError as e:
            # Re-raise validation errors
            logger.error(f"Validation error: {e}")
            raise
        except Exception as e:
            logger.error(f"Error setting headlight state: {e}")
            raise

