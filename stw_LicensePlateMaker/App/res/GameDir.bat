@echo off
REM �Ƃ����̂̓R�����g�BLua�Ō��� -- <-�R��

REM �V���[�g�J�b�g�����ꏊ�B�f�t�H���g�ł̓f�X�N�g�b�v
set shortCutPath=%HOMEPATH%\Desktop

REM =====Steam�̏ꏊ�����W�X�g������擾=====
REM �Ԃ����Ⴏ�����͊C�O�̐l���������̃R�s�y

Set "steamPath="
For /F "Tokens=1,2*" %%A In ('Reg Query HKCU\SOFTWARE\Valve\Steam') Do (
    If "%%A" Equ "SteamPath" Set "steamPath=%%C")
If Not Defined steamPath Exit/B

REM �X���b�V�����o�b�N�X���b�V���ɒu������
set steamPath=%steamPath:/=\%


REM =====�X�g���̏ꏊ��T��=====

set gamePath=notFound

REM �X�g����ID
set gid="573090"

REM ���͂����ɃQ�[���̃C���X�g�[���悪�ۑ�����Ă���B�Ԃ����Ⴏ�������C�O�f�����瓾�����
set steamPath=%steamPath%\steamapps\libraryfolders.vdf

REM echo %steamPath%

REM 1�s�����Ă���
for /f "skip=1 delims=	 tokens=1-2" %%a in ('type "%steamPath%"') do ( 
    REM echo %%a %%b
    
    REM ����path�Ƃ������ڂ�����Εۑ�
    if %%a=="path" set gamePath=%%b\\steamapps\\common\\Stormworks

    REM �L�ڂ��Ă���ID���X�g���ƈ�v������goto�Ń��[�v�𔲂���
    if %%a==%gid% goto :rEnd
)
:rEnd

REM �]�v�ȃ_�u���N�H�[�e�[�V���������
set gamePath=%gamePath:"=%

REM �G�X�P�[�v�V�[�P���X�ɂȂ��Ă���̂𒼂�
set gamePath=%gamePath:\\=\%

REM %gamePath% �̒��Ƀp�X�������Ă���
REM echo Game path is %gamePath%

REM �p�X��ۑ�
@echo %gamePath%> res\GamePath.txt

exit