import sys, os, urllib

#get the csv file from google finance
tickerSymbol= raw_input('Enter the ticker symbol: ')
startDate = raw_input('Enter the start date(Ex. Jan 20, 2015): ')
endDate = raw_input('Enter the end date: ')
startDate.replace (" ", "+")
startDate.replace (",", "2C")
endDate.replace (" ", "+")
endDate.replace (",", "2C")

url = "http://www.google.com/finance/historical?q="+str(tickerSymbol)+"&startdate="+str(startDate)+"&enddate="+str(endDate)+"&output=csv"
 

urllib.urlretrieve(url, str(tickerSymbol))
if os.path.isfile(str(tickerSymbol)):
       os.rename(str(tickerSymbol), str(tickerSymbol)+".csv")
       print ("--File Fetched--")
       sys.exit()

print ("--Could not find file--")


#rearrange so latest date is at the bottom of the list
#stock = str(tickerSymbol)+' '+str(startDate)+' '+str(endDate)
#open('csv_out.csv', 'w').writelines(open(stock, 'r').readlines()[::-1])
