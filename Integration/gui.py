from easygui import * 

title = "Begin"
option =["Set Zero","Move","Pick Up"]
option1 =["Type A","Type B","Type C"]
option2 =["Pattern 1","Pattern 2","Pattern 3"]
button = buttonbox("Choose a mode",title=title,choices=option)
button1 = buttonbox("Choose a type",title=title,choices=option1)
button2 = buttonbox("Choose a pattern",title=title,choices=option2)

msgbox(msg= button,title=title )


