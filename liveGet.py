import urllib2  # works fine with Python 2.7.9 (not 3.4.+)
import json
import time
import sys
import sysv_ipc
 
def fetchPreMarket(symbol, exchange):
    link = "http://finance.google.com/finance/info?client=ig&q="
    url = link+"%s:%s" % (exchange, symbol)
    try:
        u = urllib2.urlopen(url)
    except:
       print ("Could not find stock "+str(symbol)+" on exchange "+str(exchange))
       sys.exit()

    content = u.read()
    data = json.loads(content[3:])

    info = data[0]

    t = str(info["lt"])    # time stamp (elt for after hours and pre market)
    l = str(info['l'])    # market price 
    c = str(info['c'])   #change in price
    cp = str(info['cp']) #change in %
  
    return (t,l,c,cp)
 

time.sleep(2)
symbol= sys.argv[1]
exchange = sys.argv[2]
t, l, c, cp = fetchPreMarket(symbol,exchange)
#print("%s\t%.2f" % (t, l))



memory = sysv_ipc.SharedMemory(101)
#memory_value = memory.read()



memory.write(t,0)
memory.write(l,25)
memory.write(c,50)
memory.write(cp,75)




#http://www.stuffaboutcode.com/2013/08/shared-memory-c-python-ipc.html
#http://qsb-mac-plugins.googlecode.com/svn-history/r4/trunk/stock-quoter/trunk/StockQuoter.py what google finance api mean
#http://dev.markitondemand.com/MODApis/Api/v2/Quote/jsonp?symbol=AAPL&callback=myFunction
#http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.quotes%20where%20symbol%20in%20(%22AAPL%22)&env=store://datatables.org/alltableswithkeys
#http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.quotes%20where%20symbol%20in%20(%22AAPL%22)&env=store://datatables.org/alltableswithkeys&format=json
#http://finance.yahoo.com/webservice/v1/symbols/YHOO/quote?format=json&view=detail
#http://finance.yahoo.com/webservice/v1/symbols/GSK,AAPL/quote?format=json&view=%E2%80%8C%E2%80%8Bdetail