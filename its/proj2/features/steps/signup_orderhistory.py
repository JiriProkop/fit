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

@given(u'Register Account page is displayed')
def step_impl(context):
    url = "http://opencart:8080/en-gb?route=account/register"
    context.driver.get(url)
    wait = WebDriverWait(context.driver, 15)
    wait.until(expected_conditions.url_to_be(url))


@when(u'user enters valid data')
def step_impl(context):    
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Yahoo")
    context.driver.find_element(By.ID, "input-lastname").send_keys("IsDead")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("stilll@yahoo.com")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("oknotok")
    context.driver.find_element(By.NAME, "agree").click()


@when(u'click Continue')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-primary").click()


@then(u'Your Account Has Been Created! page is shown')
def step_impl(context):
    notexpected_url = "http://opencart:8080/en-gb?route=account/register"
    wait = WebDriverWait(context.driver, 15)
    wait.until(expected_conditions.url_changes(notexpected_url))
    assert context.driver.current_url != notexpected_url, f"Dind't expected URL {notexpected_url}, got {context.driver.current_url}."
    # clean
    context.driver.get("http://opencart:8080/en-gb?route=common/home")
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_DOWN)
    element = context.driver.find_element(By.CSS_SELECTOR, ".col:nth-child(3) button:nth-child(3)")
    element.send_keys(Keys.PAGE_DOWN)
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, ".col-sm-3:nth-child(4) li:nth-child(1) > a").click()
    context.driver.find_element(By.LINK_TEXT, "Logout").click()


@when(u'user enters invalid data')
def step_impl(context):
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Hello")
    context.driver.find_element(By.ID, "input-lastname").send_keys("MrInvalid")
    context.driver.find_element(By.ID, "input-email").click()
    context.driver.find_element(By.ID, "input-email").send_keys("thismaildoesntexist.com")
    context.driver.find_element(By.ID, "input-password").send_keys("uhdufdf")
    context.driver.find_element(By.NAME, "agree").click()


@then(u'Your Account Has Been Created! page is NOT shown')
def step_impl(context):
    expected_url = "http://opencart:8080/en-gb?route=account/register"
    wait = WebDriverWait(context.driver, 5)
    try:
        wait.until(expected_conditions.url_changes(expected_url))
    except:
        pass
    assert context.driver.current_url == expected_url, f"Expected URL {expected_url}, got {context.driver.current_url}."

@given(u'user at product page')
def step_impl(context):
    url = "http://opencart:8080/en-gb/product/desktops/mac/imac"
    context.driver.get(url)
    wait = WebDriverWait(context.driver, 15)
    wait.until(expected_conditions.url_to_be(url))

@when(u'user putts it in cart')
def step_impl(context):
    context.driver.find_element(By.ID, "button-cart").click()

@when(u'checks out with register')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, "a > .fa-cart-shopping").click()
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn").click()
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Ohnoagain")
    context.driver.find_element(By.ID, "input-lastname").send_keys("HereThere")
    context.driver.find_element(By.ID, "input-email").send_keys("ohoh@g.com")
    context.driver.find_element(By.CSS_SELECTOR, ".row-cols-1:nth-child(2) > .col:nth-child(2)").click()
    context.driver.find_element(By.ID, "input-shipping-address-1").click()
    context.driver.find_element(By.ID, "input-shipping-address-1").send_keys("BrokenTown")
    dropdown = context.driver.find_element(By.ID, "input-shipping-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Ceredigion']").click()
    element = context.driver.find_element(By.ID, "input-shipping-zone")
    element.send_keys(Keys.PAGE_DOWN)
    actions = ActionChains(context.driver)
    time.sleep(0.1)
    actions.move_to_element(element).click_and_hold().perform()
    element = context.driver.find_element(By.ID, "input-shipping-zone")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.ID, "input-shipping-zone")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).release().perform()
    context.driver.find_element(By.ID, "input-shipping-postcode").click()
    context.driver.find_element(By.ID, "input-shipping-postcode").send_keys("10212")
    context.driver.find_element(By.ID, "input-shipping-city").click()
    context.driver.find_element(By.ID, "input-shipping-city").send_keys("NightCity")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("sdsdsddsd")
    context.driver.find_element(By.ID, "input-register-agree").click()
    context.driver.find_element(By.ID, "button-register").click()
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_UP)
    dropdown = context.driver.find_element(By.ID, "input-shipping-method")
    dropdown.find_element(By.XPATH, "//option[. = 'Flat Shipping Rate - $8.00']").click()
    dropdown = context.driver.find_element(By.ID, "input-payment-method")
    dropdown.find_element(By.XPATH, "//option[. = 'Cash On Delivery']").click()
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_DOWN)
    context.driver.find_element(By.ID, "button-confirm").click()

@when(u'goes to Order History')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "history").click()

@then(u'said product is there')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-info").click()
    