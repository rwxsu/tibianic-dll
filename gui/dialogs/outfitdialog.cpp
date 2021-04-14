#include "outfitdialog.h"
#include "networkmessage.h"
#include "main.h"
#include "gui.h"

#define INTERNAL_PALETTE_X 160
#define INTERNAL_PALETTE_Y 36 
#define INTERNAL_PALETTE_D 13

#define INTERNAL_OUTFIT_X 30
#define INTERNAL_OUTFIT_Y INTERNAL_PALETTE_Y - 2
#define INTERNAL_OUTFIT_D 64

#define HSI_SI_VALUES 7
#define HSI_H_STEPS 19

void GUIOutfitDialog::chooseNone(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	for(int i = 1; i <= dialog->getControl<Buttons>(CONTROL_1_LOCATION)->getAmount(); i++){
		Button* control = dialog->getControl<Buttons>(CONTROL_1_LOCATION)->getButton(i);
		if(button != control){
			control->deactivate();
		}
	}
}

void GUIOutfitDialog::chooseHead(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	/* Activate this button in case of deactivation */
	button->activate();
	
	/* Deselect other buttons */
	GUIOutfitDialog::chooseNone(button, param);
	
	/* Choose mask pointer */
	dialog->m_mask = &dialog->m_outfit.masks.head;
}

void GUIOutfitDialog::chooseBody(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	/* Activate this button in case of deactivation */
	button->activate();
	
	/* Deselect other buttons */
	GUIOutfitDialog::chooseNone(button, param);
	
	/* Choose mask pointer */
	dialog->m_mask = &dialog->m_outfit.masks.body;
}

void GUIOutfitDialog::chooseLegs(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	/* Activate this button in case of deactivation */
	button->activate();
	
	/* Deselect other buttons */
	GUIOutfitDialog::chooseNone(button, param);
	
	/* Choose mask pointer */
	dialog->m_mask = &dialog->m_outfit.masks.legs;
}

void GUIOutfitDialog::chooseFeet(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	/* Activate this button in case of deactivation */
	button->activate();
	
	/* Deselect other buttons */
	GUIOutfitDialog::chooseNone(button, param);
	
	/* Choose mask pointer */
	dialog->m_mask = &dialog->m_outfit.masks.feet;
}

void GUIOutfitDialog::choosePrevious(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	if(!dialog->m_outfits.empty()){
		if(dialog->m_outfitId - 1 >= 0){
			dialog->m_outfitId--;
		} else {
			dialog->m_outfitId = dialog->m_outfits.size() - 1;
		}
		
		dialog->m_outfit.type = dialog->m_outfits[dialog->m_outfitId].first;
		dialog->getControl<Label>(CONTROL_4_LOCATION)->setText(dialog->m_outfits[dialog->m_outfitId].second);
	}
}

void GUIOutfitDialog::chooseNext(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	if(!dialog->m_outfits.empty()){
		if(dialog->m_outfitId + 1 < dialog->m_outfits.size()){
			dialog->m_outfitId++;
		} else {
			dialog->m_outfitId = 0;
		}
		
		dialog->m_outfit.type = dialog->m_outfits[dialog->m_outfitId].first;
		dialog->getControl<Label>(CONTROL_4_LOCATION)->setText(dialog->m_outfits[dialog->m_outfitId].second);
	}
}

void GUIOutfitDialog::chooseAccept(Button* button, void* param){
	GUIOutfitDialog* dialog = reinterpret_cast<GUIOutfitDialog*>(param);
	
	if(Tibia::IsOnline()){ // Lets make sure we are online
		// Create network message
		NetworkMessage* message = new NetworkMessage(NetworkMessage::XTEA);
		
		// Fill network mesage
		message->AddByte(0xD3);
		message->AddU16(dialog->m_outfit.type);
		message->AddByte(dialog->m_outfit.masks.head);
		message->AddByte(dialog->m_outfit.masks.body);
		message->AddByte(dialog->m_outfit.masks.legs);
		message->AddByte(dialog->m_outfit.masks.feet);
		
		// Send the network message
		g_dll.SendXTEANetworkMessage(message);
	}
	
	/* Set dialog to none */
	GUI::nullDialog(button, param);
}

GUIOutfitDialog::GUIOutfitDialog() : GUIWindow("Choose Outfit", 0, 0, 420, 250) {
	GUIWindow::pushButton("Cancel", &GUI::nullDialog, this, getButtonNextX(), getButtonNextY());
	GUIWindow::pushButton("Ok", &GUIOutfitDialog::chooseAccept, this, getButtonNextX(), getButtonNextY());
	
	GUIParent::addButtons(CONTROL_1_LOCATION, INTERNAL_PALETTE_X - 50, INTERNAL_PALETTE_Y - 2);
	GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("Head", &GUIOutfitDialog::chooseHead, this, 0, 25 * 0, true, true);
	GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("Body", &GUIOutfitDialog::chooseBody, this, 0, 25 * 1, true, true);
	GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("Legs", &GUIOutfitDialog::chooseLegs, this, 0, 25 * 2, true, true);
	GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->pushButton("Feet", &GUIOutfitDialog::chooseFeet, this, 0, 25 * 3, true, true);
	
	GUIParent::addButtons(CONTROL_2_LOCATION, INTERNAL_PALETTE_X - 140, INTERNAL_PALETTE_Y + HSI_SI_VALUES * INTERNAL_PALETTE_D - BUTTON_HEIGHT + 2);	
	GUIParent::getControl<Buttons>(CONTROL_2_LOCATION)->pushButton("Previous", &GUIOutfitDialog::choosePrevious, this, 44 * 0, 0, false, true);
	GUIParent::getControl<Buttons>(CONTROL_2_LOCATION)->pushButton("Next", &GUIOutfitDialog::chooseNext, this, 44 * 1, 0, false, true);
	
	GUIParent::addLabelEx(CONTROL_3_LOCATION, 20, INTERNAL_PALETTE_Y + HSI_SI_VALUES * INTERNAL_PALETTE_D + 18, 340);
	GUIParent::getControl<LabelEx>(CONTROL_3_LOCATION)->setText(Label_t(FONT_BIG, 191, 191, 191, 0), "Choose an outfit and determine the colours of the various parts of your character's body\n\nQuest outfits such as faction outfits can also be found in this menu");
	
	GUIParent::addLabel(CONTROL_4_LOCATION, 20 + 43, 22);
	GUIParent::getControl<Label>(CONTROL_4_LOCATION)->setText(Label_t(FONT_BIG, 191, 191, 191, 1), "?");
	
	/* Selected outfit id in m_outfits vector */
	m_outfitId = 0;
	
	/* Choose head */
	GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->getButton(1)->activate();
	GUIParent::getControl<Buttons>(CONTROL_1_LOCATION)->getButton(1)->click();
}

void GUIOutfitDialog::setOutfits(std::vector< std::pair<uint32_t, std::string> > outfits){
	m_outfits = outfits;
	
	if(!m_outfits.empty()){
		/* Set current outfit to first on the list */
		m_outfit.type = m_outfits[0].first;
		
		/* Set name of the outfit */
		GUIParent::getControl<Label>(CONTROL_4_LOCATION)->setText(m_outfits[0].second);
	}
	
	/* Outfit id will be always the first one */
	m_outfitId = 0;
}

void GUIOutfitDialog::setOutfit(Outfit_t outfit){
	for(int i = 0; i < m_outfits.size(); ++i){
		if(m_outfits[i].first == outfit.type){
			/* Set current outfit */
			m_outfit = outfit;
			
			/* Set selected outfit id */
			m_outfitId = i;
			
			/* Set outfit name */
			GUIParent::getControl<Label>(CONTROL_4_LOCATION)->setText(m_outfits[i].second);
			
			break;
		}
	}
}

bool GUIOutfitDialog::onMouseEvent(MouseEvent_t event, int x, int y, bool leftButtonDown /*= 0*/, bool rightButtonDown /*= 0*/){
	if(GUIWindow::onMouseEvent(event, x, y, leftButtonDown, rightButtonDown)){
		return true;
	}
	
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	/* Relative position of the click action */
	int rx = x - absolute->x;
	int ry = y - absolute->y;
	
	if(isButtonClickEvent(event, leftButtonDown)){
		if(isPointInRectangle(rx, ry, INTERNAL_PALETTE_X, INTERNAL_PALETTE_Y, INTERNAL_PALETTE_D * HSI_H_STEPS - 1, INTERNAL_PALETTE_D * HSI_SI_VALUES - 1)){
			int dx = (rx - INTERNAL_PALETTE_X) / INTERNAL_PALETTE_D;
			int dy = (ry - INTERNAL_PALETTE_Y) / INTERNAL_PALETTE_D;
		
			/* Set value of the new color */
			*m_mask = dy * HSI_H_STEPS + dx;	
			
			/* There is no need to pass the action to any other window */
			return true;
		}
	}
	
	return false;
}

void GUIOutfitDialog::drawSelf(int surface){
	/* Absolute position of this element */
	Position* absolute = GUIElement::getAbsolutePos();
	
	int dx = INTERNAL_PALETTE_X;
	int dy = INTERNAL_PALETTE_Y;
	
	for(int y = 0; y < HSI_SI_VALUES; y++){
		for(int x = 0; x < HSI_H_STEPS; x++){
			int colorId = x + y * HSI_H_STEPS;
			
			if(*m_mask == colorId){
				Painter::drawSkin(surface, absolute->x + dx, absolute->y + dy, 12, 12, 23, 0, 0);
			} else {
				Painter::drawSkin(surface, absolute->x + dx, absolute->y + dy, 12, 12, 22, 0, 0);
			}
			
			Color color = Painter::getOutfitColor(colorId);
			Painter::drawRectangle(surface, absolute->x + dx + 2, absolute->y + dy + 2, 8, 8, color.red, color.green, color.blue);
			
			dx += INTERNAL_PALETTE_D;
		}
		
		dy += INTERNAL_PALETTE_D;
		dx = INTERNAL_PALETTE_X;
	}
	
	Painter::drawCreature(surface, absolute->x + INTERNAL_OUTFIT_X, absolute->y + INTERNAL_OUTFIT_Y + 4, INTERNAL_OUTFIT_D, m_outfit.type, (int*)&m_outfit.masks, 38, 35, 31, 0x00, 0x00, 0xFFFF, 0xFFFF);
	Painter::drawBorders(surface, absolute->x + 20, absolute->y + INTERNAL_OUTFIT_Y, 87, 70);
}
