// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PRINTING_PRINT_VIEW_MANAGER_H_
#define CEF_LIBCEF_BROWSER_PRINTING_PRINT_VIEW_MANAGER_H_

#include "include/internal/cef_types_wrappers.h"

#include "base/macros.h"
#include "chrome/browser/printing/print_view_manager.h"
#include "components/printing/common/print.mojom-forward.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class RenderFrameHost;
class RenderProcessHost;
class WebContentsObserver;
}  // namespace content

class CefBrowserInfo;

struct PrintHostMsg_RequestPrintPreview_Params;

namespace printing {

// CEF handler for print commands.
class CefPrintViewManager : public PrintViewManager {
 public:
  ~CefPrintViewManager() override;

  // Callback executed on PDF printing completion.
  typedef base::Callback<void(bool /*ok*/)> PdfPrintCallback;

  // Print the current document to a PDF file. Execute |callback| on completion.
  bool PrintToPDF(content::RenderFrameHost* rfh,
                  const base::FilePath& path,
                  const CefPdfPrintSettings& settings,
                  const PdfPrintCallback& callback);

  // mojom::PrintManagerHost methods:
  void GetDefaultPrintSettings(
      GetDefaultPrintSettingsCallback callback) override;
  void DidShowPrintDialog() override;
  void RequestPrintPreview(mojom::RequestPrintPreviewParamsPtr params) override;

  // content::WebContentsObserver methods:
  void RenderFrameDeleted(content::RenderFrameHost* render_frame_host) override;
  void NavigationStopped() override;
  void RenderProcessGone(base::TerminationStatus status) override;
  bool OnMessageReceived(const IPC::Message& message,
                         content::RenderFrameHost* render_frame_host) override;

  // Inline versions of the content::WebContentsUserData methods to avoid
  // ambiguous warnings due to the PrintViewManager base class also extending
  // WebContentsUserData.
  static void CreateForWebContents(content::WebContents* contents);
  static CefPrintViewManager* FromWebContents(content::WebContents* contents);
  static const CefPrintViewManager* FromWebContents(
      const content::WebContents* contents);

 private:
  explicit CefPrintViewManager(content::WebContents* web_contents);

  // IPC Message handlers.
  void OnMetafileReadyForPrinting_PrintToPdf(
      content::RenderFrameHost* rfh,
      const mojom::DidPreviewDocumentParams& params,
      const mojom::PreviewIds& ids);
  void TerminatePdfPrintJob();

  // Used for printing to PDF. Only accessed on the browser process UI thread.
  int next_pdf_request_id_ = content::RenderFrameHost::kNoFrameTreeNodeId;
  struct PdfPrintState;
  std::unique_ptr<PdfPrintState> pdf_print_state_;

  DISALLOW_COPY_AND_ASSIGN(CefPrintViewManager);
};

}  // namespace printing

#endif  // CEF_LIBCEF_BROWSER_PRINTING_PRINT_VIEW_MANAGER_H_