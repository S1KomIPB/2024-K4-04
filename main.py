from fastapi import FastAPI
from typing import Dict
import sqlite3

from models import SensorData


app = FastAPI()

db = "test.db"
table = "sensordata"
startup_script = f"""
    CREATE TABLE IF NOT EXISTS {table} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        api_key TEXT,
        sensor TEXT,
        temperature REAL,
        humidity REAL
    )
"""
get_script = f"SELECT * FROM {table}"
post_script = f"""
        INSERT INTO {table} (api_key, sensor, temperature, humidity)
        VALUES (?, ?, ?, ?)
"""

@app.on_event("startup")
async def startup_event():
    app.state.conn = sqlite3.connect(db)
    cursor = app.state.conn.cursor()
    cursor.execute(startup_script)
    app.state.conn.commit()
    cursor.close()

@app.on_event("shutdown")
async def shutdown_event():
    app.state.conn.close()


@app.get("/")
async def get():
    cursor = app.state.conn.cursor()
    cursor.execute(get_script)
    rows = cursor.fetchall()
    cursor.close()
    for row in rows:
        print(row)
    return rows


@app.post("/post")
async def post_data(data: SensorData):
    data = data.dict()
    cursor = app.state.conn.cursor()
    cursor.execute(post_script, (data["api_key"], data["sensor"], data["temperature"], data["humidity"]))
    app.state.conn.commit()
    cursor.close()
    return {"message": "Data saved successfully"}
