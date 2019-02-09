#!/usr/bin/env python3
from datetime import datetime, timedelta
import os, sys, requests, caldav, bs4, uuid, mechanicalsoup, subprocess, re
from caldav.elements import dav, cdav
from lxml import etree

browser = mechanicalsoup.StatefulBrowser()
browser.open("https://www.peoplestuffuk.com/WFMMCDPRD/Login.jsp")

browser.select_form('form[action="/WFMMCDPRD/LoginSubmit.jsp"]')
browser["txtUserID"] = ""
browser["txtPassword"] = ""
response = browser.submit_selected()

page = browser.get_current_page()
qrygenshifts_arr = page.find_all(id="queryGenStr")[0].get('value').split(',')
qrygenshifts_len = len(qrygenshifts_arr)
qrygenshifts = ""
for i, x in enumerate(qrygenshifts_arr):
    qrygenshifts += x[1:-1]
    if not i == qrygenshifts_len - 1:
        qrygenshifts += "NXT"
genshiftid = page.find_all(id='genshiftid')[0].get('value')

path_to_schedule = ""
dt = datetime.today()
start = dt - timedelta(days=dt.weekday())
end = start + timedelta(days=20)
url = "https://www.peoplestuffuk.com/WFMMCDPRD/rws/ess/print/printSchedulePDF.jsp?newwin=Y&pageSize=A4&dispPeriod={}%20to%20{}&personid=388353&personname=%20Watson%20,%20%20George%20&genshiftid={}&qrygenshifts={}&editweek={}_{}".format(start.strftime('%d/%m/%Y'), end.strftime('%d/%m/%Y'), genshiftid, qrygenshifts, dt.strftime('%Y'), int(dt.strftime('%V')) + 1) 
r = requests.post(url, stream=True, cookies=requests.utils.dict_from_cookiejar(browser.get_cookiejar()))
if r.status_code == 200:
    with open(path_to_schedule, 'wb') as f:
        for chunk in r:
            f.write(chunk)
else:
    print("Failed to get schedule: %d" % r.status_code)
    sys.exit(-1)

schedule = subprocess.run(['/usr/local/bin/pdftotext', '-table', path_to_schedule, '-'], stdout=subprocess.PIPE).stdout.decode('utf8')
# os.remove(path_to_schedule)
browser.close()

dates = []
reg = re.compile(r'^\S{3}\s+(\d+\/\d+\/\d+)\s+\d+\s+(\d+:\d+)-(\d+:\d+)\s+\d+:\d+\s+(\d+:\d+)$')
for line in schedule.split("\n"):
    m = reg.match(line)
    if m:
        dates.append((datetime.strptime(m.group(1), "%d/%m/%Y"), m.group(2), m.group(3), m.group(4)))
if not len(dates):
    sys.exit(0)

username = ""
password = ""
icloud_url = "https://caldav.icloud.com"
propfind_principal = (
    u'''<?xml version="1.0" encoding="utf-8"?><propfind xmlns='DAV:'>'''
    u'''<prop><current-user-principal/></prop></propfind>''')
propfind_calendar_home_set = (
    u'''<?xml version="1.0" encoding="utf-8"?><propfind xmlns='DAV:' '''
    u'''xmlns:cd='urn:ietf:params:xml:ns:caldav'><prop>'''
    u'''<cd:calendar-home-set/></prop></propfind>''')

auth = requests.auth.HTTPBasicAuth(username, password)
headers = {'Depth': '2'}
principal_response = requests.request(
        'PROPFIND', icloud_url,
        auth=auth, headers=headers,
        data=propfind_principal.encode('utf-8'))
if principal_response.status_code != 207:
    print('Failed to retrieve Principal: ', principal_response.status_code)
    sys.exit(-1)

soup = bs4.BeautifulSoup(principal_response.content, 'lxml')
principal_path = soup.find('current-user-principal').find('href').get_text()

home_set_response = requests.request(
        'PROPFIND', icloud_url + principal_path,
        auth=auth, headers=headers,
        data=propfind_calendar_home_set.encode('utf-8'))
if home_set_response.status_code != 207:
    print('Failed to retrieve calendar-home-set', home_set_response.status_code)
    sys.exit(-1)

soup = bs4.BeautifulSoup(home_set_response.content, 'lxml')
calendar_home_set_url = soup.find('href', attrs={'xmlns':'DAV:'}).get_text()

caldav = caldav.DAVClient(calendar_home_set_url, username=username, password=password)
principal = caldav.principal()
calendars = principal.calendars()

work_cal = None
if len(calendars) > 0:
    for calendar in calendars:
        properties = calendar.get_properties([dav.DisplayName(), ])
        display_name = properties['{DAV:}displayname']
        if display_name == "Work":
            work_cal = calendar
if not work_cal:
    print('Failed to retrieve work calendar')
    sys.exit(-1)

for e in work_cal.events():
    try:
        if e.data:
            e.delete()
    except:
        pass

for date in dates:
    end = date[2]
    if float(date[1].replace(":", ".")) + float(date[3].replace(":", ".")) > 24:
        end = "23:59"
    try:
        work_cal.add_event("""
BEGIN:VCALENDAR
VERSION:2.0
PRODID:-//TAKE IT EASY//TAKE IT EASY//EN
BEGIN:VEVENT
UID:{}
DTSTAMP:{}
DTSTART:{}
DTEND:{}
SUMMARY:WORK AT {}-{}
BEGIN:VALARM
X-WR-ALARMUID:{}
UID:{}
TRIGGER;VALUE=DURATION:-PT2H
DESCRIPTION:Event reminder
ACTION:DISPLAY
END:VALARM
END:VEVENT
END:VCALENDAR
""".format(str(uuid.uuid4()).upper(), dt.strftime("%Y%m%d") + "T000000", "{}T{}00".format(date[0].strftime("%Y%m%d"), date[1].replace(':', '')), "{}T{}00".format(date[0].strftime("%Y%m%d"), end.replace(':', '')), date[1], date[2], str(uuid.uuid4()).upper(), str(uuid.uuid4()).upper()))
    except:
        pass
