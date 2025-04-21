/**
 * SDV Cloud Dashboard - Main JavaScript
 */
const dashboard = (function() {
    // Private state
    let isStreaming = false;
    let eventSource = null;
    
    // DOM elements
    const domElements = {
        fuelValue: document.getElementById('fuel-value'),
        fuelLevelBar: document.getElementById('fuel-level-bar'),
        fuelTimestamp: document.getElementById('fuel-timestamp'),
        headlightState: document.getElementById('headlight-state'),
        headlightTimestamp: document.getElementById('headlight-timestamp'),
        streamToggle: document.getElementById('stream-toggle'),
        streamingLabel: document.getElementById('streaming-label'),
        eventLog: document.getElementById('event-log'),
        streamingStatus: document.querySelector('.streaming-status'),
        lastUpdate: document.getElementById('last-update')
    };
    
    /**
     * Format timestamp
     * @param {number} timestamp - Unix timestamp
     * @return {string} Formatted time string
     */
    function formatTime(timestamp) {
        const date = new Date(timestamp * 1000);
        return date.toLocaleTimeString();
    }
    
    /**
     * Add log entry to the event log
     * @param {string} message - Log message
     */
    function addLogEntry(message) {
        const now = new Date();
        const timeStr = now.toTimeString().split(' ')[0];
        const logEntry = document.createElement('div');
        logEntry.className = 'log-entry';
        logEntry.innerHTML = `<span class="log-time">[${timeStr}]</span> ${message}`;
        domElements.eventLog.appendChild(logEntry);
        domElements.eventLog.scrollTop = domElements.eventLog.scrollHeight;
    }
    
    /**
     * Request data from API
     * @param {string} dataKey - Key for the data to request
     */
    function requestData(dataKey) {
        addLogEntry(`Requesting ${dataKey} data...`);
        
        fetch('/api/request_data', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({key: dataKey})
        })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                updateFuelLevel(data.data.value);
                addLogEntry(`Received ${dataKey}: ${data.data.value}`);
                domElements.lastUpdate.textContent = new Date().toLocaleTimeString();
            } else {
                addLogEntry(`Error: ${data.error}`);
            }
        })
        .catch(error => {
            addLogEntry(`Request failed: ${error}`);
        });
    }
    
    /**
     * Get headlights state from API
     */
    function getHeadlightsState() {
        addLogEntry(`Requesting headlight state...`);
        
        fetch('/api/headlights', {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json'
            }
        })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                updateHeadlightState(data.data.value);
                addLogEntry(`Received ${data.data.key}: ${data.data.value}`);
                domElements.lastUpdate.textContent = new Date().toLocaleTimeString();
            } else {
                addLogEntry(`Error: ${data.error}`);
            }
        })
        .catch(error => {
            addLogEntry(`Request failed: ${error}`);
        });
    }

    /**
     * Set headlights state via API
     * @param {boolean} value - New headlight state
     */
    function setHeadlightsState(value) {
        addLogEntry(`Setting headlight state...`);
        
        fetch('/api/headlights', {
            method: 'PUT',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({turn_on: value})
        })
        .then(response => response.json())
        .then(data => {
            if (data.status === 'success') {
                addLogEntry(`Successfully updated ${data.data.key}: ${value}`);
                domElements.lastUpdate.textContent = new Date().toLocaleTimeString();
            } else {
                addLogEntry(`Error: ${data.error}`);
            }
        })
        .catch(error => {
            addLogEntry(`Request failed: ${error}`);
        });
    }

    /**
     * Update headlight state display
     * @param {string} value - Headlight state value
     */
    function updateHeadlightState(value) {
        addLogEntry(`Value type: ${typeof value}`);
        if (value == true) {
            domElements.headlightState.textContent = "On";
        } else {
            domElements.headlightState.textContent = "Off";
        }
        
        // Update timestamp
        domElements.headlightTimestamp.textContent = `Updated ${new Date().toLocaleTimeString()}`;
    }
    
    /**
     * Update fuel level display
     * @param {string} value - Fuel level value
     */
    function updateFuelLevel(value) {
        // Extract numeric value (assuming format like "75%")
        const numericValue = parseFloat(value);
        
        // Update text display
        domElements.fuelValue.textContent = value;
        
        // Update gauge
        domElements.fuelLevelBar.style.width = `${numericValue}%`;
        
        // Update timestamp
        domElements.fuelTimestamp.textContent = `Updated ${new Date().toLocaleTimeString()}`;
    }

    /**
     * Toggle streaming on/off
     */
    function toggleStreaming() {
        if (isStreaming) {
            // Stop streaming
            if (eventSource) {
                eventSource.close();
                eventSource = null;
            }
            
            domElements.streamToggle.textContent = 'Start Streaming';
            domElements.streamingLabel.textContent = 'Streaming stopped';
            domElements.streamingStatus.classList.remove('streaming');
            isStreaming = false;
            addLogEntry('Streaming stopped');
        } else {
            // Start streaming
            const interval = document.getElementById('stream-interval').value;
            
            // Call API to start the stream on the server
            fetch(`/api/stream/fuel_level?interval=${interval}&max_updates=10`)
            .then(response => response.json())
            .then(data => {
                if (data.status === 'success') {
                    addLogEntry(`Started streaming with interval: ${interval}s`);
                    
                    // Set up EventSource for real-time updates
                    eventSource = new EventSource('/api/events');
                    
                    eventSource.onmessage = function(event) {
                        const data = JSON.parse(event.data);
                        
                        if (data.event === 'fuel_level_update') {
                            const fuelLevel = data.data.data.level_percent + '%';
                            updateFuelLevel(fuelLevel);
                            addLogEntry(`Stream update: Fuel level ${fuelLevel}`);
                            domElements.lastUpdate.textContent = new Date().toLocaleTimeString();
                        } else if (data.event === 'error') {
                            addLogEntry(`Stream error: ${data.data.message}`);
                            toggleStreaming(); // Stop streaming on error
                        }
                    };
                    
                    domElements.streamToggle.textContent = 'Stop Streaming';
                    domElements.streamingLabel.textContent = 'Streaming active';
                    domElements.streamingStatus.classList.add('streaming');
                    isStreaming = true;
                } else {
                    addLogEntry(`Failed to start streaming: ${data.error}`);
                }
            })
            .catch(error => {
                addLogEntry(`Streaming request failed: ${error}`);
            });
        }
    }
    
    // Initialize event listeners
    function initEventListeners() {
        // Form submission handler for data requests
        document.getElementById('request-form').addEventListener('submit', function(e) {
            e.preventDefault();
            const dataKey = document.getElementById('data-key').value;
            requestData(dataKey);
        });
        
        // Form submission handler for headlight control
        document.getElementById('headlight-form').addEventListener('submit', function(e) {
            e.preventDefault();
            const stateStr = document.getElementById('data-value').value;
            const state = stateStr === "true";
            setHeadlightsState(state);
            getHeadlightsState();
        });
    }
    
    // Initialize application
    function init() {
        addLogEntry('Dashboard initialized');
        initEventListeners();
        
        // Request initial data
        setTimeout(() => {
            requestData('fuel_level');
        }, 500);
    }
    
    // Run initialization when DOM is loaded
    document.addEventListener('DOMContentLoaded', init);
    
    // Public API
    return {
        requestData: requestData,
        toggleStreaming: toggleStreaming,
        getHeadlightsState: getHeadlightsState,
        setHeadlightsState: setHeadlightsState
    };
})();