/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Laboratory for Percutaneous Surgery (PerkLab)
  Queen's University, Kingston, ON, Canada. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Csaba Pinter, PerkLab, Queen's University
  and was supported through the Applied Cancer Research Unit program of Cancer Care
  Ontario with funds provided by the Ontario Ministry of Health and Long-Term Care

==============================================================================*/

// SubjectHierarchy Plugins includes
#include "qSlicerSubjectHierarchyPluginHandler.h"
#include "qSlicerSubjectHierarchyModelsPlugin.h"
#include "qSlicerSubjectHierarchyDefaultPlugin.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

// Qt includes
#include <QDebug>
#include <QStandardItem>
#include <QAction>

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_SubjectHierarchy_Plugins
class qSlicerSubjectHierarchyModelsPluginPrivate: public QObject
{
  Q_DECLARE_PUBLIC(qSlicerSubjectHierarchyModelsPlugin);
protected:
  qSlicerSubjectHierarchyModelsPlugin* const q_ptr;
public:
  qSlicerSubjectHierarchyModelsPluginPrivate(qSlicerSubjectHierarchyModelsPlugin& object);
  ~qSlicerSubjectHierarchyModelsPluginPrivate();
  void init();
public:
  QIcon ModelIcon;

  QAction* ToggleSliceIntersectionVisibilityAction;
};

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyModelsPluginPrivate methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyModelsPluginPrivate::qSlicerSubjectHierarchyModelsPluginPrivate(qSlicerSubjectHierarchyModelsPlugin& object)
: q_ptr(&object)
, ModelIcon(QIcon(":Icons/Model.png"))
, ToggleSliceIntersectionVisibilityAction(NULL)
{
}

//------------------------------------------------------------------------------
void qSlicerSubjectHierarchyModelsPluginPrivate::init()
{
  Q_Q(qSlicerSubjectHierarchyModelsPlugin);

  this->ToggleSliceIntersectionVisibilityAction = new QAction("Toggle slice intersection visibility",q);
  QObject::connect(this->ToggleSliceIntersectionVisibilityAction, SIGNAL(toggled(bool)), q, SLOT(toggleSliceIntersectionVisibility(bool)));
  this->ToggleSliceIntersectionVisibilityAction->setCheckable(true);
  this->ToggleSliceIntersectionVisibilityAction->setChecked(false);
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyModelsPluginPrivate::~qSlicerSubjectHierarchyModelsPluginPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerSubjectHierarchyModelsPlugin methods

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyModelsPlugin::qSlicerSubjectHierarchyModelsPlugin(QObject* parent)
 : Superclass(parent)
 , d_ptr( new qSlicerSubjectHierarchyModelsPluginPrivate(*this) )
{
  this->m_Name = QString("Models");

  Q_D(qSlicerSubjectHierarchyModelsPlugin);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerSubjectHierarchyModelsPlugin::~qSlicerSubjectHierarchyModelsPlugin()
{
}

//----------------------------------------------------------------------------
double qSlicerSubjectHierarchyModelsPlugin::canAddNodeToSubjectHierarchy(
  vtkMRMLNode* node, vtkIdType parentItemID/*=vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID*/)const
{
  Q_UNUSED(parentItemID);
  if (!node)
    {
    qCritical() << Q_FUNC_INFO << ": Input node is NULL!";
    return 0.0;
    }
  else if (node->IsA("vtkMRMLModelNode"))
    {
    // Node is a model
    return 0.5;
    }
  return 0.0;
}

//---------------------------------------------------------------------------
double qSlicerSubjectHierarchyModelsPlugin::canOwnSubjectHierarchyItem(vtkIdType itemID)const
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    return 0.0;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return 0.0;
    }

  // Model
  vtkMRMLNode* associatedNode = shNode->GetItemDataNode(itemID);
  if (associatedNode && associatedNode->IsA("vtkMRMLModelNode"))
    {
    return 0.5; // There may be other plugins that can handle special models better
    }

  return 0.0;
}

//---------------------------------------------------------------------------
const QString qSlicerSubjectHierarchyModelsPlugin::roleForPlugin()const
{
  return "Model";
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyModelsPlugin::icon(vtkIdType itemID)
{
  Q_D(qSlicerSubjectHierarchyModelsPlugin);

  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    return QIcon();
    }

  if (this->canOwnSubjectHierarchyItem(itemID))
    {
    return d->ModelIcon;
    }

  // Item unknown by plugin
  return QIcon();
}

//---------------------------------------------------------------------------
QIcon qSlicerSubjectHierarchyModelsPlugin::visibilityIcon(int visible)
{
  // Have the default plugin (which is not registered) take care of this
  return qSlicerSubjectHierarchyPluginHandler::instance()->defaultPlugin()->visibilityIcon(visible);
}

//-----------------------------------------------------------------------------
QString qSlicerSubjectHierarchyModelsPlugin::tooltip(vtkIdType itemID)const
{
  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    return QString("Invalid!");
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return QString("Error!");
    }

  // Get basic tooltip from abstract plugin
  QString tooltipString = Superclass::tooltip(itemID);

  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(shNode->GetItemDataNode(itemID));
  vtkPolyData* polyData = modelNode ? modelNode->GetPolyData() : NULL;
  vtkMRMLModelDisplayNode* displayNode = modelNode ? vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode()) : NULL;
  if (modelNode && displayNode && polyData)
    {
    bool visible = (displayNode->GetVisibility() > 0);
    tooltipString.append( QString(" (Points: %1  Cells: %2  Visible: %3")
      .arg(polyData->GetNumberOfPoints()).arg(polyData->GetNumberOfCells())
      .arg(visible ? "YES" : "NO") );
    if (visible)
      {
        double color[3] = {0.0,0.0,0.0};
        displayNode->GetColor(color);
      tooltipString.append( QString("  Color: %4,%5,%6  Opacity: %7%")
        .arg(int(color[0]*255.0)).arg(int(color[1]*255.0)).arg(int(color[2]*255.0))
        .arg(int(displayNode->GetOpacity()*100.0)) );
      }
    tooltipString.append(QString(")"));
    }
  else
    {
    tooltipString.append(" !Invalid model!");
    }

  return tooltipString;
}

//---------------------------------------------------------------------------
QList<QAction*> qSlicerSubjectHierarchyModelsPlugin::visibilityContextMenuActions()const
{
  Q_D(const qSlicerSubjectHierarchyModelsPlugin);

  QList<QAction*> actions;
  actions << d->ToggleSliceIntersectionVisibilityAction;
  return actions;
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyModelsPlugin::showVisibilityContextMenuActionsForItem(vtkIdType itemID)
{
  Q_D(qSlicerSubjectHierarchyModelsPlugin);

  if (itemID == vtkMRMLSubjectHierarchyNode::INVALID_ITEM_ID)
    {
    return;
    }
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }

  // Model
  if (this->canOwnSubjectHierarchyItem(itemID))
    {
    vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(shNode->GetItemDataNode(itemID));
    if (!modelNode)
      {
      qCritical() << Q_FUNC_INFO << ": Failed to find model node associated to subject hierarchy item " << itemID;
      return;
      }
    vtkMRMLModelDisplayNode* displayNode = vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (!displayNode)
      {
      qCritical() << Q_FUNC_INFO << ": Failed to find display node for model node " << modelNode->GetName();
      return;
      }

    d->ToggleSliceIntersectionVisibilityAction->blockSignals(true);
    d->ToggleSliceIntersectionVisibilityAction->setChecked(displayNode->GetSliceIntersectionVisibility());
    d->ToggleSliceIntersectionVisibilityAction->blockSignals(false);
    d->ToggleSliceIntersectionVisibilityAction->setVisible(true);
    }
}

//---------------------------------------------------------------------------
void qSlicerSubjectHierarchyModelsPlugin::toggleSliceIntersectionVisibility(bool on)
{
  vtkMRMLSubjectHierarchyNode* shNode = qSlicerSubjectHierarchyPluginHandler::instance()->subjectHierarchyNode();
  if (!shNode)
    {
    qCritical() << Q_FUNC_INFO << ": Failed to access subject hierarchy node";
    return;
    }

  vtkIdType currentItemID = qSlicerSubjectHierarchyPluginHandler::instance()->currentItem();
  if (!currentItemID)
    {
    qCritical() << Q_FUNC_INFO << ": Invalid current item";
    return;
    }

  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast(shNode->GetItemDataNode(currentItemID));
  vtkMRMLModelDisplayNode* displayNode = modelNode ? vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode()) : NULL;
  if (displayNode)
    {
    displayNode->SetSliceIntersectionVisibility(on);
    }
}
