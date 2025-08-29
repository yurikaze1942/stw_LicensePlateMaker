@echo off
REM というのはコメント。Luaで言う -- <-コレ

REM ショートカットを作る場所。デフォルトではデスクトップ
set shortCutPath=%HOMEPATH%\Desktop

REM =====Steamの場所をレジストリから取得=====
REM ぶっちゃけここは海外の人が作ったやつのコピペ

Set "steamPath="
For /F "Tokens=1,2*" %%A In ('Reg Query HKCU\SOFTWARE\Valve\Steam') Do (
    If "%%A" Equ "SteamPath" Set "steamPath=%%C")
If Not Defined steamPath Exit/B

REM スラッシュをバックスラッシュに置き換え
set steamPath=%steamPath:/=\%


REM =====ストワの場所を探す=====

set gamePath=notFound

REM ストワのID
set gid="573090"

REM 実はここにゲームのインストール先が保存されている。ぶっちゃけここも海外掲示板から得た情報
set steamPath=%steamPath%\steamapps\libraryfolders.vdf

REM echo %steamPath%

REM 1行ずつ見ていく
for /f "skip=1 delims=	 tokens=1-2" %%a in ('type "%steamPath%"') do ( 
    REM echo %%a %%b
    
    REM もしpathという項目があれば保存
    if %%a=="path" set gamePath=%%b\\steamapps\\common\\Stormworks

    REM 記載してあるIDがストワと一致したらgotoでループを抜ける
    if %%a==%gid% goto :rEnd
)
:rEnd

REM 余計なダブルクォーテーションを取る
set gamePath=%gamePath:"=%

REM エスケープシーケンスになっているのを直す
set gamePath=%gamePath:\\=\%

REM %gamePath% の中にパスが入っている
REM echo Game path is %gamePath%

REM パスを保存
@echo %gamePath%> res\GamePath.txt

exit