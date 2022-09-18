# 一个自动切换go-cqhttp的不可靠程序

通过定时检测go-cqhttp的日志来检测bot是否被风控，若被风控，则自动切换至下一个候选。

```yml
# 检查时间间隔，单位：分钟
interval: 1
# 出现几条错误后判定为风控
error_limit: 1
# 主bot的名字，当该bot可用时，无条件切换至该bot
Bots:
  - name: Bot1
    # bot启动指令，可编写shell脚本后运行脚本完成复杂启动
    enable_cmd: echo "Bot1 start"
    # bot关闭指令，可编写shell脚本后运行脚本完成复杂关闭
    disable_cmd: echo "Bot2 stop" 
    # bot所允许的go-cqhttp的日志
    log_path: "/root/fktx/go-cqhttp_1.out"
  - name: Bot2
    enable_cmd: echo "Bot1 start"
    disable_cmd: echo "Bot2 stop"
    log_path: "/root/fktx/go-cqhttp_2.out"
```