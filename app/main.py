from fastapi import FastAPI, Depends
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from sqlalchemy.orm import Session
from datetime import datetime
from . import models, schemas, crud
from .database import engine, SessionLocal, database

models.Base.metadata.create_all(bind=engine)

app = FastAPI()
API_KEY = "supersecure"

app.mount("/static", StaticFiles(directory="./static"), name="static")

@app.on_event("startup")
async def startup():
    await database.connect()

@app.on_event("shutdown")
async def shutdown():
    await database.disconnect()

# Dependency
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

@app.post("/post")
async def post_temperature(data: schemas.SensorData, db: Session = Depends(get_db)):
    if data.api_key == API_KEY:
        sensor_data = crud.create_sensor_data(db=db, sensor_data=data)
        return {"message": "Data saved successfully", "data": sensor_data}
    return {"message": "Invalid API Key"}

@app.get("/get", response_model=list[schemas.SensorData])
async def get_sensors(skip: int = 0, limit: int = 10, db: Session = Depends(get_db)):
    sensors = crud.get_sensor_data(db=db, skip=skip, limit=limit)
    return sensors

@app.get("/", response_class=HTMLResponse)
async def read_index():
    with open("./static/templates/index.html") as f:
        return HTMLResponse(content=f.read(), status_code=200)


@app.get("/actuator", response_class=HTMLResponse)
async def read_index():
    with open("./static/templates/actuator.html") as f:
        return HTMLResponse(content=f.read(), status_code=200)
