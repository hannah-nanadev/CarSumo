#pragma once
enum class ReceiverCategories
{
	kNone = 0,
	kScene = 1 << 0,
	kPlayerCar = 1 << 1,
	kParticleSystem = 1 << 7,
	kSoundEffect = 1 << 8,
	kNetwork = 1 << 9,

	kCar = kPlayerCar
};