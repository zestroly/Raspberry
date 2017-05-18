#!/usr/bin/env python
# -*- coding: utf-8 -*-
from picamera import PiCamera
from time import sleep

camera = PiCamera();
camera.resolution = (1024, 768)
camera.start_preview();
sleep(1)
camera.rotation=180
camera.resolution = (800, 600)
sleep(5)
camera.resolution = (480, 320)
sleep(5)
camera.stop_preview();

