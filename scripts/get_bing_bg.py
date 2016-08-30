#-*-coding: utf-8 -*-

import urllib, re, os

def get_bing_wallpaper():
    url = "http://cn.bing.com"
    html = urllib.urlopen(url).read()
    if not html:
        print 'open & read bing error'
        return -1
    
    reg = re.compile(';g_img={url: "(.*?)",id', re.S)
   
    imglist = re.findall(reg, html)
    print len(imglist)
    for imgurl in imglist:
        print imgurl
        right = imgurl.rindex('/')
        name = imgurl[right+1:]
        savepath = os.path.join("C:\\Users\\0045000617\\Pictures\\", name)
        urllib.urlretrieve(imgurl, savepath)

if __name__ == '__main__':
    get_bing_wallpaper()