# شبیه سازی شبکه بی سیم

در این پروژه یک شبکه بیسیم را با استفاده از ابزار ns-3 شبیه سازی کردیم
بخشی از کد این پروژه که شامل رابطه cient 
و master بود به ما داده شده بود و این گزارش شامل این بخش ها نمیشود .

## بخش master

در اینجا ما اطلاعات را از client دریافت کرده ایم و 
برای ارسال آن به mapper در 
تابع handelRead کمی تغییر ایجاد کردیم 
و متغییر val 
داده های destinationHeader را دریافت میکند.
تابع به صورت زیر است :

[![image](https://www.linkpicture.com/q/master-new_1.png)](https://www.linkpicture.com/view.php?img=LPic644196757bcc01342889040)

در اینجا از تابع sentToMapper استفاده شده است که
اطلاعاتی که از client گرفته را 
به mapper ارسال میکند 
ابن کار را با ساخت یک packet جدید انجام میدهیم .
و تابع آن به شکل زیر پیاده سازی شده است :


[![image](https://www.linkpicture.com/q/sendMapper.png)](https://www.linkpicture.com/view.php?img=LPic64417053a5abc2110941521)


## بخش mapper

پس از آن کلاس و توابع مربوط به mapper را ایجاد میکنیم که
همانند client 
و master 
شامل یک تابع StartApplication 
و HandelRead هستند که به صورت زیر است :


[![image](https://www.linkpicture.com/q/Mapper_1.png)](https://www.linkpicture.com/view.php?img=LPic644172c2548451749282700)

[![image](https://www.linkpicture.com/q/mapperHandelReas_1.png)](https://www.linkpicture.com/view.php?img=LPic64417332470c6903866529)



پس از آن mapper ها باید تشخیص بدهند که داده ای که
از client بهش رسیده است متعلق به آن هست یا خیر 






پس از تشخیص اینکه ورودی داده شده متناظر با کدام یک از حروف الفبا است باید نتیجه را به 
صورت UDP 
به client برگرداند .
این تابع یک packet جدید ساخته 
و با استفاده از header اطلاعات مورد نظر را جابه جا میکند .
تابع استفاده شده در اینجا به شکل زیر است :


[![image](https://www.linkpicture.com/q/SendClient_1.png)](https://www.linkpicture.com/view.php?img=LPic644174ee6887d1202808681)


## بخش main 

در اینجا باید سه mapper تعریف کنیم 
این کار را با استفاده از NodeContainer انجام میدهیم :

[![image](https://www.linkpicture.com/q/NodeContainer.png)](https://www.linkpicture.com/view.php?img=LPic644176362693d1409096302)

با استفاده از WifiMacHelper 
میتوانیم WifiMac را روی گروهی از نود ها فعال کنیم :


[![image](https://www.linkpicture.com/q/mac.png)](https://www.linkpicture.com/view.php?img=LPic64417fd2b4dd235763565)


و mobiliyHelper به ما کمک میکند که محل قرارگیری نود ها را تعیین کنیم 
و InternetStackHelper در زمینه 
توانایی های TCP/UDP/IP به نودها کمک میکند :

[![image](https://www.linkpicture.com/q/Mobility.png)](https://www.linkpicture.com/view.php?img=LPic64417fd2b4dd235763565)


پس از آن با استفاده از Ipv4InterfaceContainer 
میتوانیم به هر کدام یک آدرس اختصاص بدهیم و برای هر کدام از نودها یک object بسازیم :


[![image](https://www.linkpicture.com/q/IPv4.png)](https://www.linkpicture.com/view.php?img=LPic64417fd2b4dd235763565)


[![image](https://www.linkpicture.com/q/object.png)](https://www.linkpicture.com/view.php?img=LPic64417fd2b4dd235763565)


## بخش نتایج 

در این بخش throughput را چاپ میکنیم که 
نسبت تمام packet های دریافت شده توسط نودها به فاصله زمانی
بیت دریافت اولین و اخرین packet است .
علاوه بر آن حروف متناظر با اعدادی که به عنوان ورودی داده شده اند هم قابل مشاهده است .
همچنین میتوانیم ببینیم که ip ادرس 
نود ها نمایش داده شده است برای مثال میتوان دید که از ۱۰.۱.۳.۱ به ۱۰.۱.۳.۲ بسته ارسال شده است و بعد بسته بین مپر های متفاوت جا به جا میشود تا حرف الفبای موردنظر پیدا شود و سپس نتیجه را به کلاینت ارسال میکند . با دنبال کردن ip ها 
میتوان جا به جا شدن بسته را بررسی کرد .



[![image](https://www.linkpicture.com/q/res1_1.png)](https://www.linkpicture.com/view.php?img=LPic644195a0237931341679499)

[![image](https://www.linkpicture.com/q/res2_1.png)](https://www.linkpicture.com/view.php?img=LPic644195a0237931341679499)

[![image](https://www.linkpicture.com/q/Screenshot-1073.png)](https://www.linkpicture.com/view.php?img=LPic64419da432da8927960226)