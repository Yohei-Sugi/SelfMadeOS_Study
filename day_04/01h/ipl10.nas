; hello-os-ipl
; TAB=4

CYLS	EQU		10				; �ǂ��܂œǂݍ��ނ�

		ORG		0x7c00			; ���̃v���O�������ǂ��ɓǂݍ��܂��̂�

; �ȉ��͕W���I��FAT12�t�H�[�}�b�g�t���b�s�[�f�B�X�N�̂��߂̋L�q

		JMP		entry
		DB		0x90
		DB		"YO-SUGI?"		; �u�[�g�Z�N�^�̖��O�����R�ɋL�q���Ă悢(������8�o�C�g�ɂ��邱��)
		DW		512				; �Z�N�^�̑傫��(512�o�C�g�Œ�)
		DB		1				; �N���X�^�̑傫��(1�Z�N�^�ɂ��Ȃ���΂����Ȃ�)
		DW		1				; FAT���Ƃ�����n�܂邩(�ʏ��1�Z�N�^�ڂ���ɂ���)
		DB		2				; FAT�̌�(2�ŌŒ�)
		DW		224				; ���[�g�f�B���N�g���̈�̑傫��(�ʏ��224�G���g���Ƃ���)
		DW		2880			; ���̃h���C�u�̑傫��(2880�Z�N�^�ŌŒ�)
		DB		0xf0			; ���f�B�A�̃^�C�v(0xf0�ŌŒ�)
		DW		9				; FAT�̈�̒���(9�Z�N�^�̂��Ȃ���΂����Ȃ�)
		DW		18				; 1�g���b�N�ɂ����̃Z�N�^�����邩(18�ŌŒ�)
		DW		2				; �w�b�h�̐�(2�ɂ��Ȃ���΂����Ȃ�)
		DD		0				; �p�[�e�B�V�������g���Ă��Ȃ��̂ł����͕K��0
		DD		2880			; ���̃h���C�u�̑傫�����ēx�L�q����
		DB		0,0,0x29		; ���܂��Ȃ�
		DD		0xffffffff		; �����炭�惆�[���V���A���ԍ�
		DB		"YO-SUGI?OS "	; �f�B�X�N�̖��O(11�o�C�g)
		DB		"FAT12   "		; �t�H�[�}�b�g�̖��O(8�o�C�g)
		RESB	18				; �Ƃ肠����18�o�C�g�����Ă���

; �v���O�����{��

entry:
		MOV		AX,0			; ���W�X�^�̏�����
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX

; �f�B�X�N��ǂ�
		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; �V�����_0
		MOV		DH,0			; �w�b�h0
		MOV		CL,2			; �Z�N�^2

readloop:
		MOV		SI,0			; ���s�񐔂𐔂��郌�W�X�^

retry:
		MOV		AH,0x02			; AH=0x02 : �f�B�X�N�ǂݍ���
		MOV		AL,1			; 1�Z�N�^
		MOV		BX,0
		MOV		DL,0x00			; A�h���C�u
		INT		0x13			; �f�B�X�NBIOS�Ăяo��
		JNC		next			; �G���[���N���Ȃ����next��
		ADD		SI,1			; SI��1�𑫂�
		CMP		SI,5			; SI��5���r
		JAE		error			; SI >= 5 ��������error��
		MOV		AH,0x00
		MOV		DL,0x00			; A�h���C�u
		INT		0x13			; �h���C�u�̃��Z�b�g
		JMP		retry

next:
		MOV		AX,ES			; �A�h���X��0x200�i�߂�
		ADD		AX,0x0020
		MOV		ES,AX			; ADD ES,0x020 �Ƃ������߂��Ȃ����߈�x�AAX�Ɉڂ��K�v������
		ADD		CL,1			; CL��1�𑫂�
		CMP		CL,18			; CL��18���r
		JBE		readloop		; CL <= 18 ��������readloop��
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH < 2 ��������readloop��
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS ��������readloop��

; �ǂݏI������̂ŁAharibote.sys�����s
		MOV		[0x0ff0],CH		; IPL���ǂ��܂œǂ񂾂̂�������
		JMP		0xc200

error:
		MOV		SI,msg

putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI��1�𑫂�
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; �ꕶ���\���t�@���N�V����
		MOV		BX,15			; �J���[�R�[�h
		INT		0x10			; �r�f�IBIOS�Ăяo��
		JMP		putloop

fin:
		HLT						; �Ȃɂ�����܂�CPU���~������
		JMP		fin				; �������[�v

msg:
		DB		0x0a, 0x0a		; ���s��2��
		DB		"load error"
		DB		0x0a			; ���s
		DB		0

		RESB	0x7dfe-$		; 0x7dfe�܂ł�0x00�Ŗ��߂閽��

		DB		0x55, 0xaa
