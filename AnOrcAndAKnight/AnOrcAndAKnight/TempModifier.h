#pragma once
class Fighter;

class TempModifier {
public:
	TempModifier(Fighter& target, short duration) : _target(target), _duration(duration) {};
	~TempModifier() {};

protected:
	short _duration;
	Fighter& _target;

public:
	virtual void Affect() = 0;
	virtual void Expire() = 0;
	short Update();
};


class DamageModifier : public TempModifier {
public:
	DamageModifier(Fighter& target, short duration, int value) : TempModifier(target, duration), _value(value) {};
	virtual ~DamageModifier() {};

private:
	int _value = 1;

public:
	virtual void Affect() final;
	virtual void Expire() final;
};

