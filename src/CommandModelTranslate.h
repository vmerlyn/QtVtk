#ifndef COMMANDMODELTRANSLATE_H
#define COMMANDMODELTRANSLATE_H

#include <memory>

#include "CommandModel.h"


class Model;
class QVTKFramebufferObjectRenderer;

class CommandModelTranslate : public CommandModel
{
public:
	struct TranslateParams_t {
		std::shared_ptr<Model> model;
		int screenX;
		int screenY;
		double previousPositionX;
		double previousPositionY;
		double targetPositionX;
		double targetPositionY;
		TranslateParams_t()
			: model(nullptr), screenX(0), screenY(0), previousPositionX(0), 
			previousPositionY(0), targetPositionX(0), targetPositionY(0) {}
	};

	CommandModelTranslate(QVTKFramebufferObjectRenderer *vtkFboRenderer, const TranslateParams_t & translateVector, bool inTransition);

	bool isReady() const override;
	void execute() override;

private:
	void transformCoordinates();

	TranslateParams_t m_translateParams;
	bool m_inTransition;
	bool m_needsTransformation = true;
};

#endif // COMMANDMODELTRANSLATE_H
