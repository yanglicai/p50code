@echo off

color 0A

echo 启动编译文件的清理.......


del *.bak /s
del *.ddk /s
del *.edk /s
del *.lst /s
del *.lnp /s
del *.mpf /s
del *.mpj /s
del *.obj /s
del *.omf /s
del *.plg /s
del *.rpt /s
del *.tmp /s
del *.__i /s
del *.crf /s
del *.o /s
del *.d /s
del *.tra /s
del *.dep /s
del JLinkLog.txt /s
del *.iex /s
del *.sct /s
del *.map /s
del *.htm /s
del *._2i /s
del *.cscc_bk /s
del *.xml /s

echo 预测和限制可控性空间.......


@pause