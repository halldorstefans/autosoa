from flask import Blueprint, render_template
from models.vehicle_data import VehicleDataStore

views_bp = Blueprint('views', __name__)
data_store = VehicleDataStore()

@views_bp.route('/')
def index():
    """Render the dashboard UI"""
    vehicle_data = data_store.get_all()
    return render_template('dashboard.html', data=vehicle_data)