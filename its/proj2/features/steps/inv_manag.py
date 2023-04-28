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


@given(u'loged in as administrator')
def step_impl(context):
    pass

@given(u'Products page is shown')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration/index.php?route=catalog/product&user_token=4158b1294f342e46bdb9328ba5e9da41")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.ID, "input-password").send_keys(Keys.ENTER)
    

@when(u'admin clicks Add New icon')
def step_impl(context):
    element = context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()

@when(u'admin fills in all required information')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn > .fa-plus").click()
    context.driver.find_element(By.ID, "input-name-1").click()
    context.driver.find_element(By.ID, "input-name-1").send_keys("Nonsense")
    context.driver.find_element(By.ID, "input-meta-title-1").click()
    context.driver.find_element(By.ID, "input-meta-title-1").send_keys("Monitor")
    context.driver.find_element(By.LINK_TEXT, "Data").click()
    context.driver.find_element(By.ID, "input-model").click()
    context.driver.find_element(By.ID, "input-model").send_keys("1st gen")
    context.driver.find_element(By.LINK_TEXT, "Links").click()
    context.driver.find_element(By.LINK_TEXT, "Attribute").click()
    context.driver.find_element(By.LINK_TEXT, "Option").click()
    context.driver.find_element(By.LINK_TEXT, "Subscription").click()
    context.driver.find_element(By.LINK_TEXT, "Discount").click()
    context.driver.find_element(By.LINK_TEXT, "Special").click()
    context.driver.find_element(By.LINK_TEXT, "Image").click()
    context.driver.find_element(By.LINK_TEXT, "Reward Points").click()
    context.driver.find_element(By.LINK_TEXT, "SEO").click()
    context.driver.find_element(By.CSS_SELECTOR, ".nav-item:nth-child(12) > .nav-link").click()
    context.driver.find_element(By.LINK_TEXT, "Report").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".fa-floppy-disk")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, ".fa-floppy-disk")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.LINK_TEXT, "General").click()
    context.driver.find_element(By.LINK_TEXT, "Data").click()
    context.driver.find_element(By.LINK_TEXT, "Links").click()
    context.driver.find_element(By.LINK_TEXT, "Attribute").click()
    context.driver.find_element(By.LINK_TEXT, "Option").click()
    context.driver.find_element(By.LINK_TEXT, "Subscription").click()
    context.driver.find_element(By.LINK_TEXT, "Discount").click()
    context.driver.find_element(By.LINK_TEXT, "Special").click()
    context.driver.find_element(By.LINK_TEXT, "Image").click()
    context.driver.find_element(By.LINK_TEXT, "Reward Points").click()
    context.driver.find_element(By.LINK_TEXT, "SEO").click()
    context.driver.find_element(By.ID, "input-keyword-0-1").click()
    context.driver.find_element(By.ID, "input-keyword-0-1").send_keys("/nonsense")
    context.driver.find_element(By.CSS_SELECTOR, ".nav-item:nth-child(12) > .nav-link").click()
    context.driver.find_element(By.LINK_TEXT, "Report").click()
    context.driver.find_element(By.LINK_TEXT, "SEO").click()
    

@when(u'clicks Save icon')
def step_impl(context):
    element = context.driver.find_element(By.CSS_SELECTOR, ".btn-light")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary").click()
    element = context.driver.find_element(By.CSS_SELECTOR, ".float-end > .btn-primary")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()


@then(u'that product is added')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb?route=common/home")
    context.driver.find_element(By.NAME, "search").click()
    context.driver.find_element(By.NAME, "search").send_keys("nonsense")
    context.driver.find_element(By.NAME, "search").send_keys(Keys.ENTER)
    context.driver.find_element(By.LINK_TEXT, "Nonsense").click()


@given(u'Products page with at least 1 product is shown')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration/")
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.ID, "input-password").send_keys(Keys.ENTER)
    context.driver.find_element(By.LINK_TEXT, "Catalog").click()
    context.driver.find_element(By.LINK_TEXT, "Products").click()


@when(u'admin chooses product/s')
def step_impl(context):
    # actions.move_to_element(element).perform()
    element = context.driver.find_element(By.CSS_SELECTOR, "body")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()

@then(u'product is deleted')
def step_impl(context):
    try:
        context.driver.get("http://opencart:8080/en-gb/product/apple-cinema?search=Apple+Cinema+30%22")
        # always fail, if you get here
        assert False == True 
    except:
        # this is good, the page should not exist
        pass
