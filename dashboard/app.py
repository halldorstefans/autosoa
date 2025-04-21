from flask import Flask
from routes.api import api_bp
from routes.views import views_bp
from config import Config
import logging


def create_app(config_class=Config):
    
    logging.basicConfig(level=logging.DEBUG)
    
    app = Flask(__name__)
    app.config.from_object(config_class)
    
    # Register blueprints
    app.register_blueprint(api_bp, url_prefix='/api')
    app.register_blueprint(views_bp)
    
    return app

if __name__ == '__main__':
    app = create_app()
    app.run(host='0.0.0.0', port=app.config['PORT'], debug=app.config['DEBUG'])