from behave import *
import time
import json
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.desired_capabilities import DesiredCapabilities


@given(u'Order details page is shown')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.ID, "input-password").send_keys(Keys.ENTER)
    context.driver.find_element(By.LINK_TEXT, "Sales").click()
    context.driver.find_element(By.LINK_TEXT, "Orders").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".btn-primary:nth-child(1)")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary:nth-child(1)").click()

  
@given(u'admin changes Order status to canceled')
def step_impl(context):
    element = context.driver.find_element(By.ID, "button-invoice")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.ID, "button-invoice")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    element.send_keys(Keys.PAGE_DOWN)
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    dropdown = context.driver.find_element(By.ID, "input-order-status")
    dropdown.find_element(By.XPATH, "//option[. = 'Canceled']").click()
    element = context.driver.find_element(By.ID, "input-order-status")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).click_and_hold().perform()
    element = context.driver.find_element(By.ID, "input-order-status")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.ID, "input-order-status")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).release().perform()


@given(u'clicks Add history')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_DOWN)
    time.sleep(0.1)
    context.driver.find_element(By.ID, "button-history").click()
    element = context.driver.find_element(By.ID, "button-history")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    element.send_keys(Keys.PAGE_UP)
    time.sleep(0.1)
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()


@then(u'order status is changed to canceled')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_UP)
    time.sleep(0.1)
    context.driver.find_element(By.CSS_SELECTOR, "tbody .text-start:nth-child(5)").click()
    dropdown = context.driver.find_element(By.ID, "input-order-status")
    dropdown.find_element(By.XPATH, "//option[. = 'Canceled']").click()
    context.driver.find_element(By.ID, "button-filter").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".btn-primary:nth-child(1)")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, ".fa-eye").click()


@given(u'Orders page is shown')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.ID, "input-password").send_keys(Keys.ENTER)
    context.driver.find_element(By.LINK_TEXT, "Sales").click()
    context.driver.find_element(By.LINK_TEXT, "Orders").click()


@when(u'admin chooses order/s')
def step_impl(context):
    context.driver.find_element(By.NAME, "selected[]").click()


@when(u'clicks Delete icon')
def step_impl(context):
    element = context.driver.find_element(By.ID, "button-delete")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.ID, "button-delete").click()


@when(u'confirms')
def step_impl(context):
    assert context.driver.switch_to.alert.text == "Are you sure?"
    context.driver.switch_to.alert.accept()


@then(u'order is deleted')
def step_impl(context):
    pass


