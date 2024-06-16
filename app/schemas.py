# app/schemas.py
from pydantic import BaseModel

class SensorData(BaseModel):
    api_key: str
    sensor: str
    temperature: float
    light: float
    date: str
    time: str
    distance: float

