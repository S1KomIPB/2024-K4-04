from pydantic import BaseModel


class SensorData(BaseModel):
    api_key: str
    sensor: str
    temperature: float
    humidity: float