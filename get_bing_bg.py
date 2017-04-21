#!/usr/bin/python

import urllib,re,os
import datetime

def get_datetime():
    return datetime.datetime.now().strftime("%Y%m%d")

def get_prefix():
    return "bing_" + get_datetime() + "_"

def get_name(imgurl):
    right = imgurl.rindex('/')
    name = imgurl[right+1:]
    name = get_prefix() + name
    return name

def get_bing_bg():
    url = 'http://cn.bing.com/'
    html = urllib.urlopen(url).read()
    if not html:
        return -1
    reg = re.compile(';g_img={url: "(.*?)",id',re.S)
    imglist = re.findall(reg, html)
    for imgurl in imglist:
        name = get_name(imgurl)
        if not imgurl.startswith('http'):
            imgurl = url + imgurl
        savepath = os.path.join("/home/david/Pictures/", name)
        print imgurl, '--->', savepath
        urllib.urlretrieve(imgurl, savepath)


if __name__ == '__main__':
    get_bing_bg()
