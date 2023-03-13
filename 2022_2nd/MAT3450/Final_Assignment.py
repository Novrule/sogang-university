import telepota
import requests
from telepot.loop import MessageLoop
from bs4 import BeautifulSoup
from telepot.namedtuple import InlineKeyboardMarkup
from telepot.namedtuple import InlineKeyboardButton

todo=0
index=0

def get_now_value(code):
    url = "https://finance.naver.com/item/sise.naver?code=" + code    
    html = requests.get(url).text
    soup = BeautifulSoup(html, "html5lib")
    tag = soup.select("[id=_nowVal]")
    if not tag:
        return -7777777
    else:
        return tag[0].text

def get_dvr(code):
    url = "https://finance.naver.com/item/main.nhn?code=" + code
    html = requests.get(url).text
    soup = BeautifulSoup(html, "html5lib")
    tag = soup.select("[id=_dvr]")
    if not tag:
        return -7777777
    else:
        return tag[0].text

def get_per(code):
    url = "https://finance.naver.com/item/main.nhn?code=" + code
    html = requests.get(url).text
    soup = BeautifulSoup(html, "html5lib")
    tag = soup.select("[id=_per]")
    if not tag:
        return -7777777
    else:
        return tag[0].text

def get_pbr(code):
    url = "https://finance.naver.com/item/main.nhn?code=" + code
    html = requests.get(url).text
    soup = BeautifulSoup(html, "html5lib")
    tag = soup.select("[id=_pbr]")
    if not tag:
        return -7777777
    else:
        return tag[0].text
    
def get_posibility(code):
    posibility=0
    data=0
    url ="https://finance.naver.com/item/main.nhn?code=" + code
    html = requests.get(url).text
    soup = BeautifulSoup(html, "html5lib")
    tag1 = soup.select("[id=_dvr]")
    tag2 = soup.select("[id=_per]")
    tag3 = soup.select("[id=_pbr]")
    if not tag1:
        data=data+1
    if not tag2:
        data=data+1
    if not tag3:
        data=data+1
    if (data==0):
        if (float(tag1[0].text)>2.2):
            posibility=posibility+1
        if (float(tag2[0].text)>14):
            posibility=posibility+1
        if (float(tag3[0].text)>0.9):
            posibility=posibility+1
        return posibility
    elif (data==3):
        return -777
    else:
        return -7777777

def handle(msg):
    global todo
    global index
    global chat_id
    content_type, chat_type, chat_id = telepot.glance(msg)
    if (todo==0):
        todo=1
        btn_how = InlineKeyboardButton(text="사용법 보기", callback_data="사용법")
        btn_now = InlineKeyboardButton(text="현재가 보기", callback_data="현재가")
        btn_dvr = InlineKeyboardButton(text="배당수익률 보기", callback_data="dvr")
        btn_per = InlineKeyboardButton(text="주가수익비율 보기", callback_data="per")
        btn_pbr = InlineKeyboardButton(text="주가순자산비율 보기", callback_data="pbr")
        btn_posibility = InlineKeyboardButton(text="투자가능성 보기", callback_data="posibility")
        markupBtn = InlineKeyboardMarkup(inline_keyboard=[[btn_how],[btn_now],[btn_dvr],[btn_per],[btn_pbr],[btn_posibility]])
        bot.sendMessage(chat_id, "원하시는 서비스를 선택하세요", reply_markup=markupBtn)           
    else:
        if (content_type == "text"):
            user_input=msg["text"]
            menu=user_input[:2]
            code=user_input[:6]
            if (len(user_input)!=2 and len(user_input)!=6):
                index=7
            if (menu=="메뉴"):
                index=6
            if (index==1):
                now_value=get_now_value(code)
                if (now_value == -7777777):
                    send_msg="올바른 종목 코드를 입력해주세요"
                else:
                    index=7
                    send_msg="해당 종목의 현재 가격은 %s원입니다" %(now_value)
                bot.sendMessage(chat_id, send_msg)
            elif (index==2):
                dvr=get_dvr(code)
                if (dvr == -7777777):
                    send_msg="올바른 종목 코드를 입력해주세요"
                else:
                    index=7
                    send_msg="해당 종목의 배당수익률은 %s%%입니다" %(dvr)
                bot.sendMessage(chat_id, send_msg)
            elif (index==3):
                per=get_per(code)
                if (per == -7777777):
                    send_msg="올바른 종목 코드를 입력해주세요"
                else:
                    index=7
                    send_msg="해당 종목의 주가수익비율은 %s배입니다" %(per)
                bot.sendMessage(chat_id, send_msg)
            elif (index==4):
                pbr=get_pbr(code)
                if (pbr == -7777777):
                    send_msg="올바른 종목 코드를 입력해주세요"
                else:
                    index=7
                    send_msg="해당 종목의 주가순자산비율은 %s배입니다" %(pbr)
                bot.sendMessage(chat_id, send_msg)
            elif (index==5):
                posibility=get_posibility(code)
                if (posibility == 0) :
                    index=7
                    send_msg="투자를 추천드리지 않습니다"
                elif (posibility == 1) :
                    index=7
                    send_msg="투자를 고려해볼만 하지 않습니다"
                elif (posibility == 2) :
                    index=7
                    send_msg="투자를 고려해볼만 합니다"
                elif (posibility == 3) :
                    index=7
                    send_msg="투자를 추천드립니다"
                elif (posibility == -7777777):
                    index=7
                    send_msg="자료가 부족해 분석을 할 수 없습니다"
                else:
                    send_msg="올바른 종목 코드를 입력해 주세요"
                bot.sendMessage(chat_id, send_msg)
            elif (index==6):
                todo=0
                index=7
                send_msg='''메뉴를 다시 불러옵니다
아무런 말이나 입력해주세요'''
                bot.sendMessage(chat_id, send_msg)
            else:
                send_msg="올바르지 않은 사용법입니다"
                bot.sendMessage(chat_id, send_msg)
                
def query_ans(msg):
    global todo
    global index
    query_data = msg["data"]
    if (query_data == "사용법"):
        todo=1
        bot.sendMessage(chat_id, text = '''현재 가격이 알고 싶으면
"현재가항목 클릭 후 종목코드 입력"
배당수익률이 알고 싶으면
"배당수익률항목 클릭 후 종목코드 입력"
주가수익비율이 알고 싶으면
"주가수익비율항목 클릭 후 종목코드 입력"
주가순자산비율이 알고 싶으면
"주가순자산비율항목 클릭 후 종목코드 입력"
투자 가능성을 알고 싶으면
"투자항목 클릭 후 종목코드 입력"
메뉴를 다시 불러오고 싶으면
"메뉴 입력"''')
    elif (query_data == "현재가"):
        todo=1
        index=1
        bot.sendMessage(chat_id, text = "종목 코드를 입력해주세요")
    elif (query_data == "dvr"):
        todo=1
        index=2
        bot.sendMessage(chat_id, text = "종목 코드를 입력해주세요")
    elif (query_data == "per"):
        todo=1
        index=3
        bot.sendMessage(chat_id, text = "종목 코드를 입력해주세요")
    elif (query_data == "pbr"):
        todo=1
        index=4
        bot.sendMessage(chat_id, text = "종목 코드를 입력해주세요")
    elif (query_data == "posibility"):
        todo=1
        index=5
        bot.sendMessage(chat_id, text = "종목 코드를 입력해주세요")              

token = '5955157502:AAEQG9cqQQiZafRsgb4d7mYH9_HtceznVaw'
bot = telepot.Bot(token)
MessageLoop(bot, {"chat" : handle, "callback_query" : query_ans}).run_as_thread()
