import requests

url = 'http://strela-vlna.gchd.cz/api/cash'
#url = 'http://192.168.22.7:80'
myobj = {'typ': 'overeni', 'id': '335aa91a', 'ctecka': "1"}

x = requests.post(url, json = myobj)
#x = requests.get(url)

print(x.status_code)
print(x.text)
