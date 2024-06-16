# app/models.py
from sqlalchemy import Column, Integer, String, Float, DateTime
from sqlalchemy.sql import func
from .database import Base

class SensorDataDB(Base):
    __tablename__ = "sensordata"

    id = Column(Integer, primary_key=True, index=True)
    api_key = Column(String, index=True)
    sensor = Column(String, index=True)
    temperature = Column(Float, index=True)
    light = Column(Float, index=True)
    date = Column(String, index=True)
    time = Column(String, index=True)
    distance = Column(Float, index=True)
