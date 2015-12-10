from xbmcjson import XBMC, PLAYER_VIDEO
import json
from qrtools import QR
import requests # $ pip install requests
from PIL import Image # $ pip install pillow
from PIL import ImageFont
from PIL import ImageDraw
import urllib
baseurl='http://192.168.1.59'
imgsize=(600,600)

#Login with default xbmc/xbmc credentials
xbmc = XBMC("%s/jsonrpc"%baseurl)

#Login with custom credentials
#xbmc = XBMC("http://YOURHOST/jsonrpc", "login", "password")

albums = xbmc.AudioLibrary.GetAlbums(properties=['artist','thumbnail'])
#print albums
for album in albums["result"]["albums"]:
    
    background = Image.new('RGBA', imgsize, (255, 255, 255, 255))
    
    #print album
    nothumb = True
    if album['thumbnail'] != "":
        thum = urllib.quote_plus(album['thumbnail'])
        thumburl = "%s/image/%s"%(baseurl,thum)
        r = requests.get(thumburl, stream=True)
        r.raw.decode_content = True # Content-Encoding
        try:
            im = Image.open(r.raw) #NOTE: it requires pillow 2.8+
            
            basewidth = imgsize[0]
            wpercent = (basewidth/float(im.size[0]))
            hsize = int((float(im.size[1])*float(wpercent)))
            im = im.resize((basewidth,hsize), Image.ANTIALIAS)
            
            background.paste(im, (0,0))
            nothumb = False
        except:
            print "Error opening %s"%thumburl
    if nothumb:
        font = ImageFont.truetype("Arial.ttf",14)
        background=Image.new("RGBA", (500,250),(255,255,255))
        draw = ImageDraw.Draw(background)
        draw.text((10, 10),album['label'],(0,0,0),font=font)
        draw.text((10, 50),', '.join(album['artist']),(0,0,0),font=font)
        draw = ImageDraw.Draw(background)
    
    qr = QR(data="%s"%album['albumid'], pixel_size=5)
    qr.encode(filename="/tmp/qr.png")
    img = Image.open("/tmp/qr.png")

    background.paste(img, (background.size[0]-img.size[0],background.size[1]-img.size[1]))
    background.save("/tmp/%s.png"%album['albumid'])