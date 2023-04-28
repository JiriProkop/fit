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


@given(u'homepage is displayed')
def step_impl(context):
    context.driver.get(context.homepage)


@when(u'user enters "samsung" into the search bar')
def step_impl(context):
    context.driver.find_element(By.NAME, "search").click()
    context.driver.find_element(By.NAME, "search").send_keys("samsung")
    context.driver.find_element(By.NAME, "search").send_keys(Keys.ENTER)
    

@then(u'Samsung products are shown')
def step_impl(context):
    element = context.driver.find_element(By.ID, "button-grid")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.LINK_TEXT, "Samsung SyncMaster 941BW").click()

# ---------------------------------------------------------------------------------
@given(u'all laptops and notebooks are shown')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb/catalog/laptop-notebook")
    

@when(u'user clicks Sort by Price(Low > High)')
def step_impl(context):
    dropdown = context.driver.find_element(By.ID, "input-sort")
    dropdown.find_element(By.XPATH, "//option[. = 'Price (Low > High)']").click()
    element = context.driver.find_element(By.ID, "input-sort")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).click_and_hold().perform()
    element = context.driver.find_element(By.ID, "input-sort")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.ID, "input-sort")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).release().perform()


@then(u'products are sorted from cheapest to the most expensive')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "HP LP3065").click()

# ---------------------------------------------------------------------------------
@given(u'product page is displayed')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb/product/mp3-players/ipod-classic")


@when(u'user adds it to cart')
def step_impl(context):
    context.driver.find_element(By.ID, "button-cart").click()
    element = context.driver.find_element(By.ID, "button-cart")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, "a > .fa-cart-shopping").click()
    

@when(u'clicks Checkout')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn").click()


@when(u'fills in all needed information')
def step_impl(context):
    context.driver.find_element(By.ID, "input-guest").click()
    context.driver.find_element(By.ID, "input-firstname").click()
    context.driver.find_element(By.ID, "input-firstname").send_keys("Uhoh")
    context.driver.find_element(By.ID, "input-lastname").send_keys("fuj")
    context.driver.find_element(By.ID, "input-email").send_keys("oknotok@ok.com")
    context.driver.find_element(By.ID, "input-shipping-address-1").click()
    context.driver.find_element(By.ID, "input-shipping-address-1").send_keys("Bye1")
    context.driver.find_element(By.ID, "shipping-address").click()
    context.driver.find_element(By.ID, "input-shipping-company").click()
    context.driver.find_element(By.ID, "input-shipping-postcode").click()
    context.driver.find_element(By.ID, "input-shipping-postcode").send_keys("102112")
    dropdown = context.driver.find_element(By.ID, "input-shipping-zone")
    dropdown.find_element(By.XPATH, "//option[. = 'Clackmannanshire']").click()
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
    context.driver.find_element(By.ID, "input-shipping-city").click()
    context.driver.find_element(By.ID, "input-shipping-city").send_keys("CityofTears")
    context.driver.find_element(By.ID, "button-register").click()
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_UP)
    dropdown = context.driver.find_element(By.ID, "input-shipping-method")
    dropdown.find_element(By.XPATH, "//option[. = 'Flat Shipping Rate - $8.00']").click()
    dropdown = context.driver.find_element(By.ID, "input-payment-method")
    dropdown.find_element(By.XPATH, "//option[. = 'Cash On Delivery']").click()
    context.driver.find_element(By.CSS_SELECTOR, "body").send_keys(Keys.PAGE_DOWN)
    time.sleep(0.1) # sometimes it click too soon
    context.driver.find_element(By.ID, "button-confirm").click()


@then(u'Your order has been placed! page is displayed')
def step_impl(context):
    expected_url = "http://opencart:8080/en-gb?route=checkout/success"
    wait = WebDriverWait(context.driver, 15)
    wait.until(expected_conditions.url_to_be(expected_url))
    assert context.driver.current_url == expected_url, f"Expected URL {expected_url}, got {context.driver.current_url}."

# ---------------------------------------------------------------------------------
@when(u'sets quantity which is not in stock')
def step_impl(context):
    context.driver.find_element(By.NAME, "quantity").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(3)")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.NAME, "quantity").send_keys("9999999999")
    context.driver.find_element(By.CSS_SELECTOR, ".fa-rotate").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".fa-rotate")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()


@then(u'checkout won\'t be displayed')
def step_impl(context):
    notexpected_url = "http://opencart:8080/en-gb?route=checkout/success"
    wait = WebDriverWait(context.driver, 3)
    try:
        wait.until(expected_conditions.url_to_be(notexpected_url))
    except:
        # timed out
        pass
    assert context.driver.current_url != notexpected_url, f"Dind't expected URL {notexpected_url}, got {context.driver.current_url}."
