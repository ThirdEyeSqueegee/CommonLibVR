#pragma once

#include "RE/ActiveEffect.h"  // ActiveEffect
#include "RE/ActorProcessManager.h"  // ActorProcessManager
#include "RE/ActorState.h"  // ActorState
#include "RE/ActorValueOwner.h"  // ActorValueOwner
#include "RE/ActorValues.h"  // ActorValue, ActorValue8
#include "RE/BGSEntryPointPerkEntry.h"  // BGSEntryPointPerkEntry
#include "RE/BSTArray.h"  // BSTSmallArray
#include "RE/BSTEvent.h"  // BSTEventSink
#include "RE/BSTList.h"  // BSSimpleList
#include "RE/FormTypes.h"  // BGSPerk, TESRace, TESFaction
#include "RE/IPostAnimationChannelUpdateFunctor.h"  // IPostAnimationChannelUpdateFunctor
#include "RE/MagicTarget.h"  // MagicTarget
#include "RE/TESNPC.h"  // TESNPC
#include "RE/TESObjectREFR.h"  // TESObjectREFR


namespace RE
{
	class ActorMover;
	class BaseExtraList;
	class bhkCharacterMoveFinishEvent;
	class BSTransformDeltaEvent;
	class MovementControllerNPC;
	class PerkEntryVisitor;


	class Actor :
		public TESObjectREFR,								// 000
		public MagicTarget,									// 098
		public ActorValueOwner,								// 0B0
		public ActorState,									// 0B8
		public BSTEventSink<BSTransformDeltaEvent>,			// 0C8
		public BSTEventSink<bhkCharacterMoveFinishEvent>,	// 0D0
		public IPostAnimationChannelUpdateFunctor			// 0D8
	{
	private:
		using EntryPointType = BGSEntryPointPerkEntry::EntryPointType;

	public:
		enum { kTypeID = FormType::ActorCharacter };


		struct SlotTypes
		{
			enum
			{
				kLeftHand = 0,
				kRightHand,
				kUnknown,
				kPowerOrShout,

				kNumSlots
			};
		};


		enum class Flag1 : UInt32
		{
			kNone = 0,
			kAIEnabled = 1 << 1,
			kIsPlayerTeammate = 1 << 26,
			kIsGuard = 1 << 30
		};


		enum class Flag2 : UInt32
		{
			kNone = 0,
			kHasInteraction = 1 << 1,
			kNoBleedoutRecovery = 1 << 5,
			kCanDoFavor = 1 << 7,
			kAllowBleedoutDialogue = 1 << 9,
			kIsTrespassing = 1 << 12,
			kIsInKillMove = 1 << 14,
			kAttackMeOnSight = 1 << 15,
			kIsCommandedActor = 1 << 16,
			kIsEssential = 1 << 18,
			kIsProtected = 1 << 19,
			kDontShowOnStealthMeter = 1 << 26,
		};


		enum class CaughtState : UInt32
		{
			kError = static_cast<std::underlying_type_t<CaughtState>>(-1),
			kUncaught = 0,
			kCaught = 1
		};


		struct ActorValueModifiers
		{
			struct Modifiers
			{
				enum
				{
					kPermanent,
					kTemporary,
					kDamage,
					kTotal
				};
			};


			// members
			float modifiers[Modifiers::kTotal];	// 0
		};
		STATIC_ASSERT(sizeof(ActorValueModifiers) == 0xC);


		struct RecordFlags
		{
			enum RecordFlag : UInt32
			{
				kDeleted = 1 << 5,
				kStartsDead = 1 << 9,
				kPersistent = 1 << 10,
				kInitiallyDisabled = 1 << 11,
				kIgnored = 1 << 12,
				kNoAIAcquire = 1 << 25,
				kDontHavokSettle = 1 << 29
			};
		};


		struct ActorValueMap
		{
			template<typename T>
			struct LocalMap
			{
				T* operator[](ActorValue8 a_actorValue)
				{
					if (actorValues && entries) {
						UInt32 idx = 0;
						while (actorValues[idx] != (ActorValue8)0) {
							if (actorValues[idx] == a_actorValue) {
								break;
							}
							++idx;
						}
						if (actorValues[idx] != (ActorValue8)0) {
							return &entries[idx];
						}
					}
					return 0;
				}


				// members
				ActorValue8*	actorValues;	// 00
				T*				entries;		// 08
			};
			STATIC_ASSERT(sizeof(LocalMap<float>) == 0x10);


			// members
			LocalMap<float>					baseValues;	// 00
			LocalMap<ActorValueModifiers>	modifiers;	// 10
		};
		STATIC_ASSERT(sizeof(ActorValueMap) == 0x20);


		class FactionVisitor
		{
		public:
			virtual bool Accept(TESFaction* a_faction, SInt8 a_rank) = 0;
		};


		virtual	~Actor();																																																							// 000

		// override (TESObjectREFR)
		virtual void							SaveBuffer(BGSSaveFormBuffer* a_buf) override;																																						// 00E
		virtual void							LoadBuffer(BGSLoadFormBuffer* a_buf) override;																																						// 00F
		virtual void							InitItem() override;																																												// 013
		virtual void							SetFlag00000020(bool a_set) override;																																								// 023
		virtual BGSLocation*					GetEditorLocation() override;																																										// 03C
		virtual bool							HasKeyword(BGSKeyword* a_keyword) override;																																							// 048
		virtual BGSScene*						GetCurrentScene() const override;																																									// 04A
		virtual void							SetCurrentScene(BGSScene* a_scene) override;																																						// 04B
		virtual UInt32&							RemoveItem(UInt32& a_droppedItemHandle, TESForm* a_item, UInt32 a_count, RemoveType a_mode, BaseExtraList* a_extraList, TESObjectREFR* a_moveToRef, void* a_arg7 = 0, void* a_arg8 = 0) override;	// 056
		virtual void							GetMarkerPosition(NiPoint3* a_pos) override;																																						// 05B
		virtual MagicCaster*					GetMagicCaster(UInt32 a_slot) const override;																																						// 05C
		virtual MagicTarget*					GetMagicTarget() const override;																																									// 05D
		virtual BSFaceGenAnimationData*			GetFaceGenAnimationData();																																											// 063
		virtual NiPoint3*						GetBoundLeftFrontBottom(NiPoint3* a_out) const override;																																			// 073
		virtual NiPoint3*						GetBoundRightBackTop(NiPoint3* a_out) const override;																																				// 074
		virtual void							ResetInventory(bool a_unk) override;																																								// 08A
		virtual bool							SendEquipEvent(uintptr_t a_arg1, uintptr_t a_arg2) override;																																		// 0A1

		// override (MagicTarget)
		virtual Actor*							GetMagicTargetActor() const override;																																								// 002
		virtual bool							IsActorMagicTarget() const override;																																								// 003
		virtual BSSimpleList<ActiveEffect*>*	GetActiveEffects() override;																																										// 007

		// add
		virtual void							Unk_A2(void);																																														// 0A2
		virtual void							PlaySounds(TESForm* a_item, bool a_isPickup, bool a_unk);																																			// 0A3
		virtual void							Unk_A4(void);																																														// 0A4
		virtual void							Unk_A5(void);																																														// 0A5
		virtual void							DrawSheatheWeapon(bool a_draw);																																										// 0A6
		virtual void							Unk_A7(void);																																														// 0A7
		virtual void							Unk_A8(void);																																														// 0A8
		virtual void							Unk_A9(void);																																														// 0A9
		virtual void							Unk_AA(void);																																														// 0AA
		virtual void							Unk_AB(void);																																														// 0AB
		virtual void							Unk_AC(void);																																														// 0AC
		virtual void							Unk_AD(void);																																														// 0AD
		virtual void							Unk_AE(void);																																														// 0AE
		virtual void							Unk_AF(void);																																														// 0AF
		virtual void							Unk_B0(void);																																														// 0B0
		virtual void							Unk_B1(void);																																														// 0B1
		virtual void							OnItemEquipped(bool a_playAnim);																																									// 0B2
		virtual void							Unk_B3(void);																																														// 0B3
		virtual void							Unk_B4(void);																																														// 0B4
		virtual void							SetBounty(TESFaction* a_faction, bool a_bViolent, SInt32 a_gold);																																	// 0B5
		virtual void							ModBounty(TESFaction* a_faction, bool a_bViolent, SInt32 a_gold);																																	// 0B6
		virtual void							AddCrimeGold(UInt32 a_gold, bool a_bViolent, TESFaction* a_faction);																																// 0B7
		virtual UInt32							GetBounty(TESFaction* a_faction);																																									// 0B8
		virtual void							Unk_B9(void);																																														// 0B9
		virtual void							ServeJailTime();																																													// 0BA
		virtual void							PayBounty(TESFaction* a_faction, bool a_removeStolenItems, bool a_goToJail);																														// 0BB
		virtual UInt32							IsCannibalizing();																																													// 0BC
		virtual void							Unk_BD(void);																																														// 0BD
		virtual UInt32							IsVampireFeeding();																																													// 0BE
		virtual void							Unk_BF(void);																																														// 0BF
		virtual void							Unk_C0(void);																																														// 0C0
		virtual void							Unk_C1(void);																																														// 0C1
		virtual void							Unk_C2(void);																																														// 0C2
		virtual void							Unk_C3(void);																																														// 0C3
		virtual void							Unk_C4(void);																																														// 0C4
		virtual void							Unk_C5(void);																																														// 0C5
		virtual void							Unk_C6(void);																																														// 0C6 - pure - How deep the class is in the inheritance hierarchy? Character=(return 1) PlayerCharacter=(return 2)
		virtual void							Unk_C7(void);																																														// 0C7
		virtual void							Unk_C8(void);																																														// 0C8
		virtual void							Unk_C9(void);																																														// 0C9
		virtual void							OnArmorActorValueChanged();																																											// 0CA
		virtual void							DropItem(UInt32* a_droppedItemHandle, TESForm* a_akItem, BaseExtraList* a_extraList, UInt32 a_count, UInt32 a_arg5, UInt32 a_arg6);																	// 0CB
		virtual void							PickUpItem(TESObjectREFR* a_item, UInt32 a_count, bool a_arg3, bool a_playSound);																													// 0CC
		virtual void							Unk_CD(void);																																														// 0CD
		virtual void							Unk_CE(void);																																														// 0CE
		virtual void							Unk_CF(void);																																														// 0CF
		virtual void							Unk_D0(void);																																														// 0D0
		virtual void							Unk_D1(void);																																														// 0D1
		virtual void							Unk_D2(void);																																														// 0D2
		virtual void							Unk_D3(void);																																														// 0D3
		virtual void							Unk_D4(void);																																														// 0D4
		virtual void							Unk_D5(void);																																														// 0D5
		virtual void							Unk_D6(void);																																														// 0D6
		virtual void							Unk_D7(void);																																														// 0D7
		virtual void							Unk_D8(void);																																														// 0D8
		virtual void							Unk_D9(void);																																														// 0D9
		virtual void							Unk_DA(void);																																														// 0DA
		virtual void							Unk_DB(void);																																														// 0DB
		virtual void							Unk_DC(void);																																														// 0DC
		virtual void							Unk_DD(void);																																														// 0DD
		virtual void							Unk_DE(void);																																														// 0DE
		virtual void							Unk_DF(void);																																														// 0DF
		virtual void							Unk_E0(void);																																														// 0E0
		virtual void							Unk_E1(void);																																														// 0E1
		virtual void							Unk_E2(void);																																														// 0E2
		virtual bool							IsInCombat();																																														// 0E3
		virtual void							Unk_E4(void);																																														// 0E4
		virtual void							Unk_E5(void);																																														// 0E5
		virtual void							Unk_E6(void);																																														// 0E6
		virtual void							Unk_E7(void);																																														// 0E7
		virtual void							Unk_E8(void);																																														// 0E8
		virtual void							Unk_E9(void);																																														// 0E9
		virtual void							Unk_EA(void);																																														// 0EA
		virtual void							Unk_EB(void);																																														// 0EB
		virtual void							Unk_EC(void);																																														// 0EC
		virtual void							Unk_ED(void);																																														// 0ED
		virtual void							Unk_EE(void);																																														// 0EE
		virtual void							OnWeaponSwing();																																													// 0EF
		virtual void							Unk_F0(void);																																														// 0F0
		virtual void							Unk_F1(void);																																														// 0F1
		virtual void							Unk_F2(void);																																														// 0F2
		virtual void							Unk_F3(void);																																														// 0F3
		virtual void							Unk_F4(void);																																														// 0F4
		virtual void							Unk_F5(void);																																														// 0F5
		virtual void							Unk_F6(void);																																														// 0F6
		virtual void							AdvanceSkill(ActorValue a_skillID, float a_points, UInt32 a_arg3, UInt32 a_arg4);																													// 0F7
		virtual void							Unk_F8(void);																																														// 0F8
		virtual bool							IsInFaction(TESFaction* faction);																																									// 0F9
		virtual void							VisitPerks(PerkEntryVisitor& a_visitor);																																							// 0FA
		virtual void							AddPerk(BGSPerk* a_perk, UInt32 a_arg2);																																							// 0FB
		virtual void							RemovePerk(BGSPerk* a_perk);																																										// 0FC
		virtual void							Unk_FD(void);																																														// 0FD
		virtual void							Unk_FE(void);																																														// 0FE
		virtual bool							CanProcessEntryPointPerkEntry(EntryPointType a_entryType);																																			// 0FF
		virtual void							VisitEntryPointPerkEntries(EntryPointType a_entryType, PerkEntryVisitor& a_visitor);																												// 100
		virtual void							Unk_101(void);																																														// 101
		virtual void							Unk_102(void);																																														// 102
		virtual void							Unk_103(void);																																														// 103
		virtual void							KillIfNoHealth(UInt32 a_unk1, float a_currentHealth);																																				// 104
		virtual void							Unk_105(void);																																														// 105
		virtual void							Unk_106(void);																																														// 106
		virtual void							Unk_107(void);																																														// 107
		virtual void							Unk_108(void);																																														// 108
		virtual void							Unk_109(void);																																														// 109
		virtual void							Unk_10A(void);																																														// 10A
		virtual void							Unk_10B(void);																																														// 10B
		virtual void							Unk_10C(void);																																														// 10C
		virtual void							Unk_10D(void);																																														// 10D
		virtual void							Unk_10E(void);																																														// 10E
		virtual void							Unk_10F(void);																																														// 10F
		virtual void							Unk_110(void);																																														// 110
		virtual void							Unk_111(void);																																														// 111
		virtual SInt32							GetCurrentShoutVariation();																																											// 112
		virtual void							SetLastRiddenHorseHandle(UInt32 a_horseRefHandle);																																					// 113
		virtual UInt32							GetLastRiddenHorseHandle(UInt32& a_outHandle);																																						// 114
		virtual void							Unk_115(void);																																														// 115
		virtual void							Unk_116(void);																																														// 116
		virtual void							Unk_117(void);																																														// 117
		virtual void							Unk_118(void);																																														// 118
		virtual void							Unk_119(void);																																														// 119
		virtual void							Unk_11A(void);																																														// 11A
		virtual void							Unk_11B(void);																																														// 11B
		virtual void							Unk_11C(void);																																														// 11C
		virtual void							Unk_11D(void);																																														// 11D
		virtual void							Unk_11E(void);																																														// 11E
		virtual void							Unk_11F(void);																																														// 11F
		virtual void							Unk_120(void);																																														// 120
		virtual void							Unk_121(void);																																														// 121
		virtual void							Unk_122(void);																																														// 122
		virtual void							Unk_123(void);																																														// 123
		virtual void							Unk_124(void);																																														// 124
		virtual void							Unk_125(void);																																														// 125
		virtual void							Unk_126(void);																																														// 126
		virtual float							IncerceptActorValueChange(UInt32 a_avIndex, float a_avChangeBy);																																	// 127

		TESForm*	GetEquippedObject(bool a_leftHand) const;
		void		UpdateSkinColor();
		void		UpdateHairColor();
		void		QueueNiNodeUpdate(bool a_updateWeight);
		bool		HasPerk(BGSPerk* a_perk) const;
		UInt16		GetLevel() const;
		void		SetRace(TESRace* a_race, bool a_isPlayer);
		void		UpdateWeaponAbility(TESForm* a_weapon, BaseExtraList* a_extraData, bool a_bLeftHand);
		void		UpdateArmorAbility(TESForm* a_armor, BaseExtraList* a_extraData);
		bool		IsHostileToActor(Actor* a_actor) const;
		void		ResetAI(UInt32 a_unk1, UInt32 a_unk2);
		bool		VisitFactions(FactionVisitor& a_visitor);
		TESNPC*		GetActorBase() const;
		TESRace*	GetRace() const;
		bool		IsBeingRidden() const;
		bool		IsCommandedActor() const;
		bool		IsEssential() const;
		bool		IsGuard() const;
		bool		IsInKillMove() const;
		bool		IsAIEnabled() const;
		bool		IsOnMount() const;
		bool		IsPlayerTeammate() const;
		bool		IsSneaking() const;
		bool		IsTrespassing() const;
		void		DispelWornItemEnchantments();
		CaughtState	SendStealAlarm(TESObjectREFR* a_refItemOrContainer, TESForm* a_stolenItem, UInt32 a_numItems, UInt32 a_value, TESForm* a_owner, bool a_allowGetBackStolenItemPackage);
		SInt32		CalcEntryValue(InventoryEntryData* a_entryData, UInt32 a_numItems, bool a_multiplyValueByRemainingItems) const;
		SInt32		GetDetectionLevel(Actor* a_target, UInt32 a_flag) const;
		bool		IsGhost() const;
		bool		IsSummoned() const;
		bool		IsRunning() const;


		// members
		Flag1						flags1;										// 0E0
		float						unk0E4;										// 0E4
		UInt32						unk0E8;										// 0E8
		UInt32						pad0EC;										// 0EC
		ActorProcessManager*		processManager;								// 0F0
		UInt32						refHandleDialogueTarget;					// 0F8
		UInt32						refHandleCombatTarget;						// 0FC
		UInt32						refHandleKiller;							// 100
		UInt32						unk104;										// 104
		float						unk108;										// 108 - init'd to -1
		UInt32						unk10C;										// 10C
		UInt32						unk110;										// 110
		UInt32						unk114;										// 114
		UInt32						unk118;										// 118
		UInt32						unk11C;										// 11C
		NiPoint3					unk120;										// 120
		UInt32						unk12C;										// 12C
		void*						unk130;										// 130
		void*						unk138;										// 138 - BGSLocation*
		ActorMover*					unk140;										// 140
		MovementControllerNPC*		unk148;										// 148
		void*						unk150;										// 150
		void*						unk158;										// 158
		UInt64						unk160;										// 160
		float						unk168;										// 168
		UInt32						unk16C;										// 16C
		UInt32						unk170;										// 170
		UInt32						unk174;										// 174 - init'd to 50
		UInt32						unk178;										// 178
		UInt32						unk17C;										// 17C - init'd to 7FFFFFFF
		UInt64						unk180;										// 180
		BSTSmallArray<SpellItem*>	addedSpells;								// 188
		MagicCaster*				magicCaster[SlotTypes::kNumSlots];			// 1A0
		MagicCaster*				equippingMagicItems[SlotTypes::kNumSlots];	// 1C0
		TESForm*					equippedShout;								// 1E0
		UInt32						unk1E8;										// 1E8
		UInt32						pad1EC;										// 1EC
		TESRace*					race;										// 1F0
		float						unk1F8;										// 1F8 - init'd to -1
		Flag2						flags2;										// 1FC
		ActorValueMap				avMap;										// 200
		UInt64						unk220;										// 220
		ActorValueModifiers			avHealth;									// 228
		ActorValueModifiers			avMagicka;									// 234
		ActorValueModifiers			avStamina;									// 240
		ActorValueModifiers			avVoicePoints;								// 24C
		float						unk258;										// 258 - init'd to -1
		UInt32						unk25C;										// 25C
		UInt64						unk260;										// 260
		float						unk268;										// 268
		float						unk26C;										// 26C
		UInt32						unk270;										// 270
		UInt32						unk274;										// 274
		UInt64						unk278;										// 278
		UInt64						unk280;										// 280
		UInt64						unk288;										// 288
		UInt64						unk290;										// 290
		UInt64						unk298;										// 298
		UInt64						unk2A0;										// 2A0
		UInt64						unk2A8;										// 2A8
	};
	STATIC_ASSERT(offsetof(Actor, addedSpells) == 0x188);
	STATIC_ASSERT(sizeof(Actor) == 0x2B0);
}
