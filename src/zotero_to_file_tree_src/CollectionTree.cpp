#include "CollectionTree.h"
#include <cassert>
#include <deque>
#include <fmt/format.h>

namespace zotfiles
{

std::shared_ptr<CollectionNode> CollectionTree::find(std::int64_t collectionID) const
{
  std::deque<std::shared_ptr<CollectionNode>> queue;
  for (auto& node: m_collectionNodes)
    queue.push_back(node);

  while (!queue.empty())
  {
    auto node = queue.front();
    queue.pop_front();
    if (node->collectionID == collectionID)
      return node;
    for (auto& childNode: node->childrenNodes)
      queue.push_back(childNode);
  }

  return nullptr;
}
bool CollectionTree::erase_collection_node(std::vector<std::shared_ptr<CollectionNode>>& collectionNodes,
                                           const CollectionNode& collectionNode)
{
  auto findIter = std::find_if(collectionNodes.cbegin(),
                               collectionNodes.cend(),
                               [&collectionNode](const auto& node) { return node->collectionID == collectionNode.collectionID; });
  if (findIter != collectionNodes.end())
  {
    collectionNodes.erase(findIter);
    return true;
  }

  return false;
}
bool CollectionTree::remove(const CollectionNode& collectionNode)
{
  if (erase_collection_node(m_collectionNodes, collectionNode))
    return true;

  std::deque<std::shared_ptr<CollectionNode>> queue;
  for (auto& node: m_collectionNodes)
  {
    queue.push_back(node);
  }

  while (!queue.empty())
  {
    auto node = queue.front();
    queue.pop_front();

    if (erase_collection_node(node->childrenNodes, collectionNode))
      return true;

    for (auto& childNode: node->childrenNodes)
    {
      queue.push_back(childNode);
    }
  }

  return false;
}
CollectionTree CollectionTree::build(std::unordered_map<std::int64_t, std::shared_ptr<CollectionNode>> collectionNodes)
{
  // Iterate over the nodes and add them as children of their parent nodes
  for (auto& [collectionID, collectionNode]: collectionNodes)
  {
    if (collectionNode->parentCollectionID != -1)
    {
      auto parentNodeIter = collectionNodes.find(collectionNode->parentCollectionID);
      if (parentNodeIter != collectionNodes.end())
      {
        parentNodeIter->second->childrenNodes.push_back(collectionNode);
      }
    }
  }

  // Add the root nodes to the tree
  CollectionTree collectionTree;
  for (auto& [collectionID, collectionNode]: collectionNodes)
  {
    if (collectionNode->parentCollectionID == -1)
    {
      collectionTree.m_collectionNodes.push_back(collectionNode);
    }
  }

  return collectionTree;
}

void CollectionTree::write_pdfs(std::filesystem::path outputDir)
{
  struct NodePathPair
  {
    CollectionNode* node{nullptr};
    std::filesystem::path relPath;
  };

  std::deque<NodePathPair> nodePathPairs;

  for (const auto& node: m_collectionNodes)
  {
    nodePathPairs.emplace_back(node.get(), std::filesystem::path(node->collectionName));
  }

  while (!nodePathPairs.empty())
  {
    auto nodePathPair = nodePathPairs.front();
    nodePathPairs.pop_front();

    for (const auto& childNode: nodePathPair.node->childrenNodes)
    {
      nodePathPairs.emplace_back(childNode.get(), nodePathPair.relPath / childNode->collectionName);
    }

    auto absCollectionDirPath = outputDir / nodePathPair.relPath;
    std::filesystem::create_directories(absCollectionDirPath);

    for (const auto& pdfItem: nodePathPair.node->collectionPDFItems)
    {
      std::filesystem::copy(pdfItem.pdfFilePath, outputDir / nodePathPair.relPath / pdfItem.pdfName);
    }
  }
}
} // namespace zotfiles