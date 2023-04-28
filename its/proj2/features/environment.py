#!/usr/bin/env python3
import time
from selenium import webdriver
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
from selenium.common.exceptions import WebDriverException
import os

def get_driver():
    '''Get Chrome/Firefox driver from Selenium Hub'''
    try:
        driver = webdriver.Remote(
                command_executor='http://localhost:4444/wd/hub',
                desired_capabilities=DesiredCapabilities.CHROME)
    except WebDriverException:
        driver = webdriver.Remote(
                command_executor='http://localhost:4444/wd/hub',
                desired_capabilities=DesiredCapabilities.FIREFOX)
    driver.implicitly_wait(15)

    return driver


def before_all(context):
    context.driver = get_driver()
    context.homepage = "http://opencart:8080/en-gb?route=common/home"
    context.driver.set_window_size(1360, 1020)
    

# po ukonceni testovani je treba zavrit driver:
# driver.close() nebo .quit()
def after_all(context):
    context.driver.quit()
    os.system("docker-compose down -v")
    os.system("docker-compose up -d")
        