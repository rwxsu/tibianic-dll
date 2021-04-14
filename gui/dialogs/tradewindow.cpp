#include "tradewindow.h"
#include "gui.h"
#include "main.h"
#include "networkmessage.h"

GUITradeWindow::GUITradeWindow() : GUIWindow("Auto Trade Messenger", 0, 0, 400, 200) {
	GUIParent::addLabel(CONTROL_1_LOCATION, m_width >> 1, 32 + 14 * 0);
	GUIParent::addLabel(CONTROL_2_LOCATION, m_width >> 1, 32 + 14 * 1);
	GUIParent::addLabel(CONTROL_3_LOCATION, 12, 32 + 14 * 4);
	
	GUIParent::addInputBox(CONTROL_4_LOCATION, 12, 32 + 14 * 5, 378);
	GUIParent::addCheckBox(CONTROL_5_LOCATION, 12, 32 + 14 * 7, 378);
	
	GUIParent::getControl<Label>(CONTROL_1_LOCATION)->setText(Label_t(FONT_BIG, 191, 191, 191, 1), "Auto trade messenger allows you to automatically"); 
	GUIParent::getControl<Label>(CONTROL_2_LOCATION)->setText(Label_t(FONT_BIG, 191, 191, 191, 1), "send message on trade channel each 3 minutes."); 
	GUIParent::getControl<Label>(CONTROL_3_LOCATION)->setText(Label_t(FONT_BIG, 191, 191, 191, 0), "Please type in the message you would like to be send on trade:");
	
	GUIParent::getControl<CheckBox>(CONTROL_5_LOCATION)->setText("Send above message on Trade Channel each three minutes");
	GUIParent::getControl<CheckBox>(CONTROL_5_LOCATION)->addListener(GUIElement::OnChange, boost::bind(&GUITradeWindow::invalidate, this, _1));
	
	GUIWindow::pushButton("Ok", &GUI::nullDialog, this, GUIWindow::getButtonNextX(), GUIWindow::getButtonNextY());
	
	/* Dialog should focus on this element */
	GUIParent::focus(GUIParent::getControl<InputBox>(CONTROL_4_LOCATION));
	
	/* Tick per 100ms */
	m_ticks = 0;
}

void GUITradeWindow::tick(uint32_t ticks){
	GUIWindow::tick(ticks);
	
	if(GUIParent::getControl<CheckBox>(CONTROL_5_LOCATION)->checked()){
		m_ticks++;
		
		if(m_ticks >= 600 * 3){
			std::string text = GUIParent::getControl<InputBox>(CONTROL_4_LOCATION)->getText();
			if(!text.empty()){ // There is no need to send empty messages
				if(Tibia::IsOnline()){ // Lets make sure we are online
					// Create network message
					NetworkMessage* message = new NetworkMessage(NetworkMessage::XTEA);
					
					// Fill network mesage
					message->AddByte(0x96);
					message->AddByte(SPEAK_CHANNEL_Y);
					message->AddU16(0x05); // Trade channel
					message->AddString(text);
					
					// Send the network message
					g_dll.SendXTEANetworkMessage(message);
				}
			}
			
			// Reset counter
			m_ticks = 0;
		}
	}
}

void GUITradeWindow::stop(){
	/* It should be called during logout */
	GUIParent::getControl<CheckBox>(CONTROL_5_LOCATION)->uncheck();
}

void GUITradeWindow::invalidate(GUIElement* caller){	
	if(GUIParent::getControl<CheckBox>(CONTROL_5_LOCATION)->checked()){
		m_ticks = 0;
	}
}
