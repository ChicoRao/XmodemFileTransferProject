////////////////////////////////////////////////
// Generated by SmartState C++ Code Generator //
//                 DO NOT EDIT				  //
////////////////////////////////////////////////

#ifndef Sender_SS_H
#define Sender_SS_H

#include <ss_api.hxx>

/*Context*/
class SenderX;

namespace Sender_SS
{
	using namespace smartstate;
	//State Mgr
	class SenderSS : public StateMgr
	{
		public:
			SenderSS(SenderX* ctx, bool startMachine=true);

			SenderX& getCtx() const;

		private:
			SenderX* myCtx;
	};

	//Base State
	class SenderBaseState : public BaseState
	{
		protected:
			SenderBaseState(){};
			SenderBaseState(const string& name, BaseState* parent, SenderSS* mgr);

		protected:
			SenderSS* getMgr(){return static_cast<SenderSS*>(myMgr);}
	};

	//States
	//------------------------------------------------------------------------
	class CompleteSenderTopLevel_SenderSS : public virtual SenderBaseState
	{
			typedef SenderBaseState super;

		public:
			CompleteSenderTopLevel_SenderSS(){};
			CompleteSenderTopLevel_SenderSS(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onTMMessage(const Mesg& mesg);
			void onSERMessage(const Mesg& mesg);
	};

	class SERcancelable_CompleteSenderTopLevel : public virtual CompleteSenderTopLevel_SenderSS
	{
			typedef CompleteSenderTopLevel_SenderSS super;

		public:
			SERcancelable_CompleteSenderTopLevel(){};
			SERcancelable_CompleteSenderTopLevel(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onKB_CMessage(const Mesg& mesg);
			void onSERMessage(const Mesg& mesg);
	};

	class EOT1_SERcancelable : public virtual SERcancelable_CompleteSenderTopLevel
	{
			typedef SERcancelable_CompleteSenderTopLevel super;

		public:
			EOT1_SERcancelable(){};
			EOT1_SERcancelable(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onSERMessage(const Mesg& mesg);
	};

	class EOTEOT_SERcancelable : public virtual SERcancelable_CompleteSenderTopLevel
	{
			typedef SERcancelable_CompleteSenderTopLevel super;

		public:
			EOTEOT_SERcancelable(){};
			EOTEOT_SERcancelable(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onSERMessage(const Mesg& mesg);
			void onTMMessage(const Mesg& mesg);
	};

	class ACKNAK_SERcancelable : public virtual SERcancelable_CompleteSenderTopLevel
	{
			typedef SERcancelable_CompleteSenderTopLevel super;

		public:
			ACKNAK_SERcancelable(){};
			ACKNAK_SERcancelable(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onSERMessage(const Mesg& mesg);
	};

	class START_SERcancelable : public virtual SERcancelable_CompleteSenderTopLevel
	{
			typedef SERcancelable_CompleteSenderTopLevel super;

		public:
			START_SERcancelable(){};
			START_SERcancelable(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onSERMessage(const Mesg& mesg);
			void onKB_CMessage(const Mesg& mesg);
	};

	class CAN_CompleteSenderTopLevel : public virtual CompleteSenderTopLevel_SenderSS
	{
			typedef CompleteSenderTopLevel_SenderSS super;

		public:
			CAN_CompleteSenderTopLevel(){};
			CAN_CompleteSenderTopLevel(const string& name, BaseState* parent, SenderSS* mgr);

			virtual void onMessage(const Mesg& mesg);

			virtual void onEntry();
			virtual void onExit();

		//Transitions

		private:
			void onTMMessage(const Mesg& mesg);
			void onSERMessage(const Mesg& mesg);
			void onKB_CMessage(const Mesg& mesg);
	};

};

#endif

//___________________________________vv^^vv___________________________________