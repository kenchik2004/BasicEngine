#pragma once
#define DEFAULT_EXCEPTION_PARAM __FILE__,__LINE__,__FUNCTION__
#define INTEGER_NAME(integer) #integer


//�f�t�H���g�̗�O���m�p�N���X
class Exception
{
public:
	//�}�N�� DEFAULT_EXCEPTION_PARAM���g�p���ď��������ł���
	//�����F
	//	���C�����b�Z�[�W�̓��e
	//	��O�����������t�@�C���̖��O
	//	��O�����������s�ԍ� 
	//	��O�����������֐���
	Exception(const char* main_message, const char* file_name, int line, const char* func_name);
	void Show();
protected:
	Exception() {}
	std::string message = "";//�ŏI�I�ɏo�͂��郁�b�Z�[�W
};

//nullptr�ւ̃A�N�Z�X���m�p��O�N���X
class NullptrException :public Exception {
public:
	NullptrException() = delete;		//�K��R���X�g���N�^�̍폜
	//�}�N���@INTEGER_NAME �� DEFAULT_EXCEPTION_PARAM���g�p���ď��������ł���
	//�����F
	//	�A�N�Z�X���悤�Ƃ����ϐ���
	//	��O�����������t�@�C���̖��O
	//	��O�����������s�ԍ� 
	//	��O�����������֐���
	NullptrException(const char* integer_name, const char* file_name, int line, const char* func_name);
	NullptrException(const char* message_);
};

//�z��O�A�N�Z�X���m�p��O�N���X
class OutOfRangeException :public Exception {
public:
	OutOfRangeException() = delete;		//�K��R���X�g���N�^�̍폜
	//�}�N���@INTEGER_NAME �� DEFAULT_EXCEPTION_PARAM���g�p���ď��������ł���
	//�����F
	//	�A�N�Z�X���悤�Ƃ����C���f�b�N�X
	//	�A�N�Z�X���悤�Ƃ����z��̃T�C�Y
	//	�A�N�Z�X���悤�Ƃ����z��̖��O
	//	��O�����������t�@�C���̖��O
	//	��O�����������s�ԍ� 
	//	��O�����������֐���
	OutOfRangeException(int index, int array_size, const char* array_name, const char* file_name, int line, const char* func_name);
};
class MemoryLeakException :public Exception{
public:
	MemoryLeakException() = delete;		//�K��R���X�g���N�^�̍폜
	//�}�N���@INTEGER_NAME �� DEFAULT_EXCEPTION_PARAM���g�p���ď��������ł���
	//�����F
	//	����ł��Ȃ������ϐ��̖��O(�܂��̓I�u�W�F�N�g�E�R���|�[�l���g��)
	//	��O�����������t�@�C���̖��O
	//	��O�����������s�ԍ� 
	//	��O�����������֐���
	MemoryLeakException(const char* integer_name, const char* file_name, int line, const char* func_name);
};

