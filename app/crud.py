# app/crud.py
from sqlalchemy.orm import Session
from datetime import datetime
from . import models, schemas

def create_sensor_data(db: Session, sensor_data: schemas.SensorData):
    db_sensor_data = models.SensorDataDB(
        api_key=sensor_data.api_key,
        sensor=sensor_data.sensor,
        temperature=sensor_data.temperature,
        light=sensor_data.light,
        date=sensor_data.date,
        time=sensor_data.time,
        distance=sensor_data.distance
    )
    db.add(db_sensor_data)
    db.commit()
    db.refresh(db_sensor_data)
    return db_sensor_data

def get_sensor_data(db: Session, skip: int = 0, limit: int = 10):
    return db.query(models.SensorDataDB).order_by(models.SensorDataDB.id.desc()).offset(skip).limit(limit).all()